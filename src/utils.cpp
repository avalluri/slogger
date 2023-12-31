/* Copyright (C) 2023 Amarnath Valluri - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: , or visit:
 * https://opensource.org/license/MIT/
 */
#include <sys/stat.h>
#include <slog/utils.h>

using namespace std;

namespace slog {
namespace utils {

// file_exists Checks if the given path is a regular file
// It does not follow symlinks.
bool file_exists(const string& path) {
    struct stat info;
    return (lstat(path.c_str(), &info) == 0) &&  ((info.st_mode & S_IFMT) == S_IFREG);
}

// directory_exists Checks if the given path is a directory.
// do not follow symlinks.
bool directory_exists(const string &path) {
    struct stat info;
    if (lstat(path.c_str(), &info) != 0) {
        // fail to stat
        return false;
    }
    if  ((info.st_mode & S_IFMT) == S_IFLNK) {
        throw "symlink occurred in the file path!";
    }
    return ((info.st_mode & S_IFMT) == S_IFDIR);
}

// is_symlink checks if the given location is a symbolic link
bool is_symlink(const string &path) {
    struct stat info;
    return (lstat(path.c_str(), &info) == 0) &&  ((info.st_mode & S_IFMT) == S_IFLNK);
}

// dirname returns the parent directory of the given
// path:
// ex(on Linux):-
//   dirname("/a/b/c/") => "/a/b/c"
//   dirname("/a/b/c") => "/a/b"
//   dirname("abc") => ""
string dirname(const string &path) {
    if (path.empty() || path == "/") {
        return "";
    }
    auto index = path.find_last_of(directory_separator);
    if (index == string::npos) {
        return "";
    }
    return path.substr(0, index);
}

bool inline create_directory(const string& dir) {
    return mkdir(dir.c_str(), mode_t(0755)) == 0;
}

// create_directory_path creates the given directory and all
//its parent directories in the path.
bool create_directory_path(const string& dir) {
    if (dir == "" || dir == directory_separator) return true;
    const string& parent_dir = dirname(dir);
    if (!parent_dir.empty()) {
        create_directory_path(parent_dir);
    }

    if ( !directory_exists(dir)) {
        return create_directory(dir);
    }
    return true;
}

// ensure_directory_path makes sure that the given directory path
// exists, if not it creates. Returns false in case the directory
// is path empty or it fails to validate/create.
//
// It raises an exception if the directory path holds a symlink
bool ensure_directory_path(const string& dir) {
    if (dir.empty() || directory_exists(dir)) {
        return true;
    }
    
    return create_directory_path(dir);
}

} // namespace utils
} // namespace slog
