
/* Copyright (C) 2023 Amarnath Valluri - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: , or visit:
 * https://opensource.org/license/MIT/
 */
#ifndef __SLOG_FILE_EXCEPTION_H_
#define __SLOG_FILE_EXCEPTION_H_

#include <string>
#include <exception>

namespace slog {

/**
 * FileException execption used for throwing exception
 * occured while initializing the FileTarget
*/
class FileException: public std::exception {
public:
    FileException(const std::string file, const std::string& msg, bool symlink=false)
        : file_(file),symlink_(symlink), error_(msg) {}

    const char *what() const noexcept{
        return error_.c_str();
    }
    
    bool isSymlink() const {
        return symlink_;
    }

    const std::string file() const {
        return file_;
    }

private:
    std::string file_;  // file name
    bool symlink_;      // is a symlink
    std::string error_; // error string

}; // class FileException

} // namespace slog

#endif //__SLOG_FILE_EXCEPTION_H_