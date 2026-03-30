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

struct FileResult {
    std::string filename;
    size_t word_count;
    size_t char_count;
    size_t line_count;
    bool success;
    std::string error_message;

    FileResult() : word_count(0), char_count(0), line_count(0), success(false) {}
};

class FileProcessor {
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
            output_file << "=== Статистика файла " << filename << " ===" << std::endl;
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
            std::cout << "[Поток " << thread_id << "] Завершил обработку файла: "
                << filename << " (время: " << duration.count() << " мкс)" << std::endl;
        }

        processed_count++;

        completion_cv.notify_one();
    }

public:
    FileProcessor(const std::vector<std::string>& files)
        : input_files(files),
        results(files.size()),
        processed_count(0),
        total_files(static_cast<int>(files.size())) {}

    void run() {
        std::cout << "Файлов для обработки: " << total_files << std::endl;

        auto global_start = std::chrono::high_resolution_clock::now();

        boost::thread_group threads;

        for (int i = 0; i < total_files; ++i) {
            threads.create_thread(boost::bind(&FileProcessor::worker_thread, this, i, input_files[i]));
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

        std::cout << "\nОбработка каждого файла вывод\n" << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            const FileResult& result = results[i];
            std::cout << "Файл: " << result.filename << std::endl;
            std::cout << "Символов: " << result.char_count << std::endl;
            std::cout << "Слов: " << result.word_count << std::endl;
            std::cout << "Строк: " << result.line_count << std::endl;
            if (!result.success) {
                std::cout << "Ошибка: " << result.error_message << std::endl;
            }
            std::cout << std::endl;
        }

        std::cout << "Обработано файлов: " << processed_count.load() << std::endl;
        std::cout << "Общее время обработки: " << global_duration.count() << " мс" << std::endl;
    }
};

void create_test_files() {

    struct TestFile {
        std::string name;
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

    for (int i = 0; i < 3; ++i) {
        std::ofstream file(test_files[i].name.c_str());
        if (file.is_open()) {
            file << test_files[i].content;
            file.close();
            std::cout << "Создан тестовый файл: " << test_files[i].name << std::endl;
        }
    }
}

void n1() {
    create_test_files();
    std::cout << std::endl;

    std::vector<std::string> files;
    files.push_back("data1.txt");
    files.push_back("data2.txt");
    files.push_back("data3.txt");

    FileProcessor processor(files);
    processor.run();
}