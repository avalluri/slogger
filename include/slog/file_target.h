#ifndef __SLOG_FILE_TARGET_H_
#define __SLOG_FILE_TARGET_H_

#include <string>
#include <mutex>
#include <stdio.h>
#include <slog/target.h>
#include <slog/utils.h>
#include <slog/file_exception.h>

using namespace std;

namespace slog {

class FileException;

/**
 * FileTargetGeneric file target used for logging to a regular file.
 * It does not accept symlinks as log target.
 * If the file is not exists then it creates a new one with
 * user-only readwrite permissions.
 * 
 * FileTarget is thread safe when it is instantiated with
 * a valid mutex(std::mutex).
 */
template <typename Mutex>
class FileTarget : public Target {
public:
    explicit FileTarget(const string& file_name, LogLevel::level_t lvl = LogLevel::Debug)
        : Target(lvl), file_name_(file_name) {

        prepare_log_file();
    }

    // Intended for creating console file targets
    explicit FileTarget(FILE *fp, bool is_console = true)
        : fp_(fp), console_(is_console) {}
    
    // Do not support copying/assigning objects
    FileTarget(const FileTarget &) = delete;
    FileTarget(FileTarget &&) = delete;
    FileTarget &operator=(const FileTarget &) = delete;
    FileTarget &operator=(FileTarget &&) = delete;


    bool log(const string& msg) override{
        lock_guard<Mutex> lock(mutex_);
        if (!fp_) return false;
        auto len = fwrite(msg.c_str(), 1, msg.size(), fp_);
        if (len < msg.size()) return false;
        // Append a new line charechter if needed
        // NOTE(avalluri): make it configurable?
        if (msg.empty() || msg[msg.size()-1] != '\n') {
            fwrite("\n", 1, 1, fp_);
        }
        return true;
    }

    void flush() override {
        lock_guard<Mutex> lock(mutex_);
        if (!fp_) return;
       ::fflush(fp_);
    }

    virtual ~FileTarget() {
        if (fp_ != nullptr && ! console_) {
            fclose(fp_);
            fp_ = nullptr;
        }
    }

protected:
    Mutex mutex_;
    string file_name_;
    FILE*  fp_{nullptr};
    bool   console_{false}; // track if the file is a console or not

private:
    void prepare_log_file() {
        if (utils::is_symlink(file_name_)) {
            throw FileException{file_name_, "Log file cannot be a symbolic link", true};
        }
        if (!utils::ensure_directory_path(utils::dirname(file_name_))) {
            throw FileException{file_name_, "Failed to create log directory"};
        }
        fp_ = fopen(file_name_.c_str(), "ab");
        if (!fp_) {
            throw FileException{file_name_, "Failed to open log file"};
        }
    }
}; // class FileTarget


// Logging target for standared output
template<class Mutex>
class StdoutTarget: public slog::FileTarget<Mutex> {
public:
    explicit StdoutTarget(): FileTarget<Mutex>(stdout) {}
    explicit StdoutTarget(LogLevel::level_t level): FileTarget<Mutex>(stdout) {
        this->SetLogLevel(level);
    }
}; // class StdoutTarget


// Logging target for standared output
template<class Mutex>
class StderrTarget: public slog::FileTarget<Mutex> {
public:
    explicit StderrTarget(): FileTarget<Mutex>(stderr) {}
    explicit StderrTarget(LogLevel::level_t level): FileTarget<Mutex>(stderr) {
        this->SetLogLevel(level);
    }
}; // class StderrTarget

} // namespace slog

#endif // __SLOG_FILE_TARGET_H_