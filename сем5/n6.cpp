#include <iostream>
#include <queue>
#include <mutex>
#include <semaphore>
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include <random>
#include <unordered_map>
#include <sstream>
#include <memory>

static std::mutex console_mutex;

struct FileChunk {
    int chunk_id;
    int file_id;
    size_t size;

    void download() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(10, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
    }
};

class FileDownload {
public:
    int file_id;
    std::vector<FileChunk> chunks;
    std::shared_ptr<std::atomic<int>> downloaded_chunks;

    FileDownload() : file_id(0), downloaded_chunks(std::make_shared<std::atomic<int>>(0)) {}

    FileDownload(int id, int num_chunks) : file_id(id), downloaded_chunks(std::make_shared<std::atomic<int>>(0)) {
        for (int i = 0; i < num_chunks; i++) {
            chunks.push_back({ i, id, 1024 });
        }
    }

    FileDownload(const FileDownload& other)
        : file_id(other.file_id), chunks(other.chunks), downloaded_chunks(other.downloaded_chunks) {
    }

    FileDownload& operator=(const FileDownload& other) {
        if (this != &other) {
            file_id = other.file_id;
            chunks = other.chunks;
            downloaded_chunks = other.downloaded_chunks;
        }
        return *this;
    }

    bool is_complete() const {
        return downloaded_chunks->load() == chunks.size();
    }

    void mark_chunk_downloaded() {
        downloaded_chunks->fetch_add(1);
    }
};

class DownloadManager {
private:
    std::queue<FileChunk> download_queue;
    std::counting_semaphore<> active_downloads;
    std::counting_semaphore<> chunk_downloads;
    std::mutex queue_mutex;
    std::atomic<int> completed_files;
    std::unordered_map<int, FileDownload> files;
    std::mutex files_mutex;
    std::atomic<bool> stop_flag;

    void safe_print(const std::string& message) {
        std::lock_guard<std::mutex> lock(console_mutex);
        std::cout << message;
    }

public:
    DownloadManager(int max_files, int max_chunks)
        : active_downloads(max_files), chunk_downloads(max_chunks), completed_files(0), stop_flag(false) {
    }

    void add_file(const FileDownload& file) {
        std::lock_guard<std::mutex> lock(files_mutex);
        files[file.file_id] = file;

        std::stringstream ss;
        ss << "  File " << file.file_id << " added with "
            << file.chunks.size() << " chunks\n";
        safe_print(ss.str());

        for (const auto& chunk : file.chunks) {
            std::lock_guard<std::mutex> qlock(queue_mutex);
            download_queue.push(chunk);
        }
    }

    inline void process_chunk(const FileChunk& chunk) {
        std::thread::id tid = std::this_thread::get_id();
        std::stringstream ss_tid;
        ss_tid << tid;
        std::string tid_str = ss_tid.str();

        std::stringstream ss;
        ss << "  Thread " << tid_str << " downloading file " << chunk.file_id
            << ", chunk " << chunk.chunk_id << "\n";
        safe_print(ss.str());

        FileChunk chunk_copy = chunk;
        chunk_copy.download();

        std::stringstream ss2;
        ss2 << "  Thread " << tid_str << " completed file " << chunk.file_id
            << ", chunk " << chunk.chunk_id << "\n";
        safe_print(ss2.str());
    }

    void download_worker() {
        while (!stop_flag) {
            FileChunk chunk;
            bool has_chunk = false;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                if (!download_queue.empty()) {
                    chunk = download_queue.front();
                    download_queue.pop();
                    has_chunk = true;
                }
            }

            if (!has_chunk) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            active_downloads.acquire();
            chunk_downloads.acquire();

            process_chunk(chunk);

            chunk_downloads.release();
            active_downloads.release();

            {
                std::lock_guard<std::mutex> lock(files_mutex);
                auto it = files.find(chunk.file_id);
                if (it != files.end()) {
                    it->second.mark_chunk_downloaded();
                    if (it->second.is_complete()) {
                        completed_files++;
                        std::stringstream ss;
                        ss << "  *** File " << chunk.file_id << " COMPLETED! ***\n";
                        safe_print(ss.str());
                    }
                }
            }

            std::this_thread::yield();
        }
    }

    void stop() {
        stop_flag = true;
    }

    int get_completed_files() const { return completed_files.load(); }
};

void n6() {
    std::cout << "Download Manager with file and chunk limits\n";
    DownloadManager manager(2, 3);

    FileDownload file1(1, 4);
    FileDownload file2(2, 3);
    FileDownload file3(3, 5);

    manager.add_file(file1);
    manager.add_file(file2);
    manager.add_file(file3);

    std::vector<std::thread> workers;
    for (int i = 0; i < 4; i++) {
        workers.emplace_back(&DownloadManager::download_worker, &manager);
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));

    manager.stop();

    for (auto& t : workers) {
        t.join();
    }

    std::cout << "  Completed files: " << manager.get_completed_files() << "\n";
}