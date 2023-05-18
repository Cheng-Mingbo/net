//
// Created by Cheng MingBo on 2023/5/14.
//
#include <sys/uio.h>  // for readv
#include <cerrno>    // for errno
#include <iostream>
#include "buffer.h"
#include <unistd.h>

void testBuffer() {
    // Create a pipe
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        std::cerr << "Failed to create pipe.\n";
        return;
    }
    
    // Write some data to the pipe
    const char* test_data = "Hello, Buffer!";
    write(pipefd[1], test_data, strlen(test_data));
    close(pipefd[1]);  // Close the write end of the pipe
    
    // Create a Buffer and use readFd() to read data from the pipe
    Buffer buffer;
    int saved_errno;
    ssize_t n = buffer.readFd(pipefd[0], &saved_errno);
    
    if (n < 0) {
        std::cerr << "readFd() failed with errno " << saved_errno << "\n";
    } else {
        // Verify that the data read from the pipe matches the data written to the pipe
        std::string_view data_view = buffer.toStringView();
        if (data_view.compare(test_data) == 0) {
            std::cout << "Test passed: readFd() successfully read data from the pipe.\n";
        } else {
            std::cout << "Test failed: readFd() read incorrect data from the pipe.\n";
        }
    }
    
    close(pipefd[0]);  // Close the read end of the pipe
}


void testBufferFunctions() {
    Buffer buffer;
    
    // Test append()
    std::string str1 = "Hello, ";
    buffer.append(str1);
    std::string str2 = "World!";
    buffer.append(str2);
    std::string_view data_view = buffer.toStringView();
    if (data_view.compare("Hello, World!") == 0) {
        std::cout << "Test passed: append() correctly appended data to the buffer.\n";
    } else {
        std::cout << "Test failed: append() did not correctly append data to the buffer.\n";
    }
    
    // Test retrieve()
    buffer.retrieve(7);
    data_view = buffer.toStringView();
    if (data_view.compare("World!") == 0) {
        std::cout << "Test passed: retrieve() correctly retrieved data from the buffer.\n";
    } else {
        std::cout << "Test failed: retrieve() did not correctly retrieve data from the buffer.\n";
    }
    
    // Test prepend()
    std::string str3 = "Hello, ";
    buffer.prepend(str3.data(), str3.size());
    data_view = buffer.toStringView();
    if (data_view.compare("Hello, World!") == 0) {
        std::cout << "Test passed: prepend() correctly prepended data to the buffer.\n";
    } else {
        std::cout << "Test failed: prepend() did not correctly prepend data to the buffer.\n";
    }
    
    // Test readableBytes(), writableBytes() and prependableBytes()
    if (buffer.readableBytes() == 13 && buffer.writableBytes() == Buffer::kInitialSize - 13 + Buffer::kCheapPrepend
        && buffer.prependableBytes() == Buffer::kCheapPrepend) {
        std::cout << "Test passed: readableBytes(), writableBytes() and prependableBytes() returned correct values.\n";
    } else {
        std::cout << "Test failed: readableBytes(), writableBytes() or prependableBytes() returned incorrect values.\n";
    }
}

int main() {
    testBuffer();
    testBufferFunctions();
    return 0;
}
