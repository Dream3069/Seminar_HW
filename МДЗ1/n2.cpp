#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <queue>
#include <map>
#include <algorithm>

struct FileResult {
    std::string filename;
    size_t word_count;
    size_t char_count;
    size_t line_count;
    bool success;
    std::string error_message;

    FileResult() : word_count(0), char_count(0), line_count(0), success(false) {}

    void print() const {
        std::cout << "Файл: " << filename << std::endl;
        std::cout << "Символов: " << char_count << std::endl;
        std::cout << "Слов: " << word_count << std::endl;
        std::cout << "Строк: " << line_count << std::endl;
        if (!success) {
            std::cout << "Ошибка: " << error_message << std::endl;
        }
    }

    FileResult& operator+=(const FileResult& other) {
        word_count += other.word_count;
        char_count += other.char_count;
        line_count += other.line_count;
        return *this;
    }
};

class FileProcessorBasic {
private:
    std::vector<std::string> input_files;
    std::vector<FileResult> results;
    boost::mutex results_mutex;
    boost::mutex output_mutex;
    boost::condition_variable completion_cv;
    std::atomic<int> processed_count;
    int total_files;

    FileResult process_single_file(const std::string& filename) {
        FileResult result;
        result.filename = filename;

        std::ifstream file(filename.c_str());
        if (!file.is_open()) {
            result.success = false;
            result.error_message = "Не удалось открыть файл";
            return result;
        }

        std::string line;
        size_t char_count = 0;
        size_t word_count = 0;
        size_t line_count = 0;

        while (std::getline(file, line)) {
            line_count++;
            char_count += line.length() + 1;

            bool in_word = false;
            for (size_t i = 0; i < line.length(); ++i) {
                char ch = line[i];
                if (std::isspace(static_cast<unsigned char>(ch))) {
                    if (in_word) {
                        word_count++;
                        in_word = false;
                    }
                }
                else {
                    in_word = true;
                }
            }
            if (in_word) {
                word_count++;
            }
        }

        result.char_count = char_count;
        result.word_count = word_count;
        result.line_count = line_count;
        result.success = true;

        std::string output_filename = "processed_" + filename;
        std::ofstream output_file(output_filename.c_str());
        if (output_file.is_open()) {
            output_file << "Статистика файла " << filename << std::endl;
            output_file << "Количество символов: " << char_count << std::endl;
            output_file << "Количество слов: " << word_count << std::endl;
            output_file << "Количество строк: " << line_count << std::endl;
            output_file.close();
        }

        return result;
    }

    void worker_thread(int thread_id, const std::string& filename) {
        {
            boost::lock_guard<boost::mutex> lock(output_mutex);
            std::cout << "[Поток " << thread_id << "] Начал обработку файла: "
                << filename << std::endl;
        }

        auto start = std::chrono::high_resolution_clock::now();
        FileResult result = process_single_file(filename);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        {
            boost::lock_guard<boost::mutex> lock(results_mutex);
            results[thread_id] = result;
        }

        {
            boost::lock_guard<boost::mutex> lock(output_mutex);
            std::cout << "[Поток " << thread_id << "] Завершил обработку: "
                << filename << " (" << duration.count() << " мкс)" << std::endl;
        }

        processed_count++;
        completion_cv.notify_one();
    }

public:
    FileProcessorBasic(const std::vector<std::string>& files)
        : input_files(files), results(files.size()), processed_count(0), total_files(static_cast<int>(files.size())) {}

    void run() {
        std::cout << "\nПоток на каждый файл" << std::endl;
        std::cout << "Файлов для обработки: " << total_files << std::endl;

        auto global_start = std::chrono::high_resolution_clock::now();

        boost::thread_group threads;
        for (int i = 0; i < total_files; ++i) {
            threads.create_thread(boost::bind(&FileProcessorBasic::worker_thread, this, i, input_files[i]));
        }

        {
            boost::unique_lock<boost::mutex> lock(results_mutex);
            completion_cv.wait(lock, [this]() {
                return processed_count.load() == total_files;
                });
        }

        threads.join_all();

        auto global_end = std::chrono::high_resolution_clock::now();
        auto global_duration = std::chrono::duration_cast<std::chrono::milliseconds>(global_end - global_start);

        std::cout << "\nОбработка каждого файла вывод" << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            results[i].print();
            std::cout << std::endl;
        }

        std::cout << "Обработано файлов: " << processed_count.load() << std::endl;
        std::cout << "Общее время обработки: " << global_duration.count() << " мс" << std::endl;
        std::cout << std::endl;
    }
};

// Очередь задач и пул потоков
class ThreadPoolProcessor {
private:
    std::queue<std::string> task_queue;
    boost::mutex queue_mutex;
    boost::condition_variable queue_cv;
    std::atomic<bool> stop_flag;
    std::atomic<int> processed_count;
    std::vector<FileResult> results;
    boost::mutex results_mutex;
    boost::mutex output_mutex;
    int total_files;
    int pool_size;

    FileResult process_single_file(const std::string& filename) {
        FileResult result;
        result.filename = filename;

        std::ifstream file(filename.c_str());
        if (!file.is_open()) {
            result.success = false;
            result.error_message = "Не удалось открыть файл";
            return result;
        }

        std::string line;
        size_t char_count = 0;
        size_t word_count = 0;
        size_t line_count = 0;

        while (std::getline(file, line)) {
            line_count++;
            char_count += line.length() + 1;

            bool in_word = false;
            for (size_t i = 0; i < line.length(); ++i) {
                char ch = line[i];
                if (std::isspace(static_cast<unsigned char>(ch))) {
                    if (in_word) {
                        word_count++;
                        in_word = false;
                    }
                }
                else {
                    in_word = true;
                }
            }
            if (in_word) {
                word_count++;
            }
        }

        result.char_count = char_count;
        result.word_count = word_count;
        result.line_count = line_count;
        result.success = true;

        std::string output_filename = "processed_" + filename;
        std::ofstream output_file(output_filename.c_str());
        if (output_file.is_open()) {
            output_file << "Количество символов: " << char_count << std::endl;
            output_file << "Количество слов: " << word_count << std::endl;
            output_file << "Количество строк: " << line_count << std::endl;
            output_file.close();
        }

        return result;
    }

    void worker_function(int worker_id) {
        while (!stop_flag.load()) {
            std::string filename;

            {
                boost::unique_lock<boost::mutex> lock(queue_mutex);
                queue_cv.wait(lock, [this]() {
                    return !task_queue.empty() || stop_flag.load();
                    });

                if (stop_flag.load() && task_queue.empty()) {
                    break;
                }

                if (!task_queue.empty()) {
                    filename = task_queue.front();
                    task_queue.pop();
                }
            }

            if (!filename.empty()) {
                {
                    boost::lock_guard<boost::mutex> lock(output_mutex);
                    std::cout << "[Рабочий " << worker_id << "] Начал обработку: "
                        << filename << std::endl;
                }

                auto start = std::chrono::high_resolution_clock::now();
                FileResult result = process_single_file(filename);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                {
                    boost::lock_guard<boost::mutex> lock(results_mutex);
                    results.push_back(result);
                }

                {
                    boost::lock_guard<boost::mutex> lock(output_mutex);
                    std::cout << "[Рабочий " << worker_id << "] Завершил: "
                        << filename << " (" << duration.count() << " мкс)" << std::endl;
                }

                processed_count++;
                queue_cv.notify_all();
            }
        }
    }

public:
    ThreadPoolProcessor(const std::vector<std::string>& files, int pool_size = 2)
        : stop_flag(false), processed_count(0), total_files(static_cast<int>(files.size())), pool_size(pool_size) {
        for (size_t i = 0; i < files.size(); ++i) {
            task_queue.push(files[i]);
        }
    }

    void run() {
        std::cout << "\nОчередь задач + Пул потоков" << std::endl;
        std::cout << "Файлов для обработки: " << total_files << std::endl;
        std::cout << "Размер пула потоков: " << pool_size << std::endl;

        auto global_start = std::chrono::high_resolution_clock::now();

        boost::thread_group workers;
        for (int i = 0; i < pool_size; ++i) {
            workers.create_thread(boost::bind(&ThreadPoolProcessor::worker_function, this, i));
        }

        {
            boost::unique_lock<boost::mutex> lock(queue_mutex);
            queue_cv.wait(lock, [this]() {
                return processed_count.load() == total_files;
                });
        }

        stop_flag = true;
        queue_cv.notify_all();
        workers.join_all();

        auto global_end = std::chrono::high_resolution_clock::now();
        auto global_duration = std::chrono::duration_cast<std::chrono::milliseconds>(global_end - global_start);

        std::cout << "\nВывод файлов" << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            results[i].print();
            std::cout << std::endl;
        }

        std::cout << "Обработано файлов: " << processed_count.load() << std::endl;
        std::cout << "Общее время обработки: " << global_duration.count() << " мс" << std::endl;
        std::cout << std::endl;

        std::cout.flush();
    }
};

// Обработка файлов по частям
class ChunkProcessor {
private:
    struct FileChunk {
        std::string filename;
        size_t start_pos;
        size_t end_pos;
        int chunk_id;
    };

    std::vector<std::string> input_files;
    std::vector<FileResult> chunk_results;
    boost::mutex results_mutex;
    boost::mutex output_mutex;
    std::atomic<int> processed_chunks;
    int total_chunks;
    static const size_t CHUNK_SIZE = 1024;

    // Обработка одного блока файла
    FileResult process_file_chunk(const FileChunk& chunk) {
        FileResult result;
        result.filename = chunk.filename + "[блок_" + std::to_string(chunk.chunk_id) + "]";

        std::ifstream file(chunk.filename.c_str(), std::ios::binary);
        if (!file.is_open()) {
            result.success = false;
            result.error_message = "Не удалось открыть файл";
            return result;
        }

        file.seekg(static_cast<std::streamoff>(chunk.start_pos));

        size_t bytes_to_read = chunk.end_pos - chunk.start_pos;
        std::vector<char> buffer(bytes_to_read);
        file.read(&buffer[0], static_cast<std::streamsize>(bytes_to_read));
        size_t bytes_read = static_cast<size_t>(file.gcount());

        size_t char_count = bytes_read;
        size_t word_count = 0;
        size_t line_count = 0;

        bool in_word = false;
        for (size_t i = 0; i < bytes_read; ++i) {
            char ch = buffer[i];

            if (ch == '\n') {
                line_count++;
            }

            if (std::isspace(static_cast<unsigned char>(ch))) {
                if (in_word) {
                    word_count++;
                    in_word = false;
                }
            }
            else {
                in_word = true;
            }
        }

        result.char_count = char_count-line_count; // вычитаем кол-во строк потому что \n считается за символ
        result.word_count = word_count;
        result.line_count = line_count;
        result.success = true;

        return result;
    }

    void chunk_worker(int worker_id, const std::vector<FileChunk>& chunks, int start_idx, int end_idx) {
        for (int i = start_idx; i < end_idx; ++i) {
            const FileChunk& chunk = chunks[i];

            {
                boost::lock_guard<boost::mutex> lock(output_mutex);
                std::cout << "[Рабочий " << worker_id << "] Обрабатывает блок "
                    << chunk.chunk_id << " файла " << chunk.filename << std::endl;
            }

            auto start = std::chrono::high_resolution_clock::now();
            FileResult result = process_file_chunk(chunk);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            {
                boost::lock_guard<boost::mutex> lock(results_mutex);
                chunk_results.push_back(result);
            }

            {
                boost::lock_guard<boost::mutex> lock(output_mutex);
                std::cout << "[Рабочий " << worker_id << "] Завершил блок "
                    << chunk.chunk_id << " (" << duration.count() << " мкс)" << std::endl;
            }

            processed_chunks++;
        }
    }

    size_t get_file_size(const std::string& filename) {
        std::ifstream file(filename.c_str(), std::ios::binary | std::ios::ate);
        return static_cast<size_t>(file.tellg());
    }

public:
    ChunkProcessor(const std::vector<std::string>& files) : input_files(files), processed_chunks(0), total_chunks(0) {}

    void run(int num_threads = 4) {
        std::cout << "\nПоблочная обработка файлов" << std::endl;
        std::cout << "Файлов для обработки: " << input_files.size() << std::endl;
        std::cout << "Количество потоков: " << num_threads << std::endl;
        std::cout << "Размер блока: " << CHUNK_SIZE << " байт" << std::endl;

        auto global_start = std::chrono::high_resolution_clock::now();

        std::vector<FileChunk> all_chunks;
        int chunk_id = 0;

        for (size_t i = 0; i < input_files.size(); ++i) {
            const std::string& filename = input_files[i];
            size_t file_size = get_file_size(filename);

            std::cout << "Файл " << filename << " размер: " << file_size << " байт" << std::endl;

            for (size_t pos = 0; pos < file_size; pos += CHUNK_SIZE) {
                FileChunk chunk;
                chunk.filename = filename;
                chunk.start_pos = pos;
                chunk.end_pos = (std::min)(pos + CHUNK_SIZE, file_size);
                chunk.chunk_id = chunk_id++;
                all_chunks.push_back(chunk);
            }
        }

        total_chunks = static_cast<int>(all_chunks.size());
        std::cout << "Всего блоков для обработки: " << total_chunks << std::endl;

        boost::thread_group workers;
        int chunks_per_thread = total_chunks / num_threads;
        int remainder = total_chunks % num_threads;

        int current_start = 0;
        for (int i = 0; i < num_threads; ++i) {
            int chunk_count = chunks_per_thread + (i < remainder ? 1 : 0);
            int current_end = current_start + chunk_count;

            if (chunk_count > 0) {
                workers.create_thread(boost::bind(&ChunkProcessor::chunk_worker, this,
                    i, all_chunks, current_start, current_end));
            }
            current_start = current_end;
        }

        workers.join_all();

        auto global_end = std::chrono::high_resolution_clock::now();
        auto global_duration = std::chrono::duration_cast<std::chrono::milliseconds>(global_end - global_start);

        std::cout << "\nВывод файлов" << std::endl;

        std::map<std::string, FileResult> aggregated_results;
        for (size_t i = 0; i < chunk_results.size(); ++i) {
            const FileResult& result = chunk_results[i];
            size_t bracket_pos = result.filename.find("[блок");
            std::string original_filename = result.filename.substr(0, bracket_pos);
            aggregated_results[original_filename] += result;
            aggregated_results[original_filename].filename = original_filename;
            aggregated_results[original_filename].success = result.success;
        }

        for (std::map<std::string, FileResult>::const_iterator it = aggregated_results.begin();
            it != aggregated_results.end(); ++it) {
            it->second.print();
            std::cout << std::endl;
        }

        std::cout << "Обработано блоков: " << processed_chunks.load() << std::endl;
        std::cout << "Общее время обработки: " << global_duration.count() << " мс" << std::endl;
        std::cout << std::endl;

    }
};

static void create_test_files() {
    struct TestFile {
        std::string filename;
        std::string content;
    };

    TestFile test_files[] = {
        {"data1.txt",
         "First file.\n"
         "Hello world!\n"
         "This is test file\n"
         "пиу пиу пау пау\n"},

        {"data2.txt",
         "Second file.\n"
         "Line 1\n"
         "Line 2\n"
         "Line 3\n"
         "Line 4\n"
         "Line 5\n"
         "Line 6\n"
         "Line 7\n"
         "Line 8\n"
         "Line 9\n"
         "Line 10\n"
         "Last line of the second file\n"},

        {"data3.txt",
         "Third file.\n"
         "блаблабла блабла бла\n"
         "бяк бяк бяк\n"
         "Надеюсь зачтут мне дз\n"}
    };

    std::cout << "Создание тестовых файлов:" << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::ofstream file(test_files[i].filename.c_str());
        if (file.is_open()) {
            file << test_files[i].content;
            file.close();
            std::cout << "Создан: " << test_files[i].filename
                << " (размер: " << test_files[i].content.length() << " байт)" << std::endl;
        }
        else {
            std::cout << "ОШИБКА: Не удалось создать " << test_files[i].filename << std::endl;
        }
    }
    std::cout << std::endl;
}

void n2() {
    std::cout << std::endl;

    create_test_files();

    std::vector<std::string> files;
    files.push_back("data1.txt");
    files.push_back("data2.txt");
    files.push_back("data3.txt");

    FileProcessorBasic basic_processor(files);
    basic_processor.run();

    boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

    // очередь задач + пул потоков
    ThreadPoolProcessor pool_processor(files, 2);
    pool_processor.run();

    boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

    // поблочная обработка
    ChunkProcessor chunk_processor(files);
    chunk_processor.run(3);
}