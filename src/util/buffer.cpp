#include "buffer.h"

#include <sys/uio.h>
#include <unistd.h>

ssize_t Buffer::readFd(int fd, int* savedErrno) {
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();
    
    // First buffer is the space left in Buffer
    vec[0].iov_base = beginWrite();
    vec[0].iov_len = writable;
    // Second buffer is the extrabuf
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);
    
    const ssize_t n = readv(fd, vec, 2);
    if (n < 0) {
        *savedErrno = errno;
    } else if (static_cast<size_t>(n) <= writable) {
        // All data fits into buffer_, no need to adjust anything
        hasWritten(n);
    } else {
        // Some data was read into extrabuf. We need to append it to buffer_
        ensureWritableBytes(n);
        append(extrabuf, n - writable);
    }
    
    return n;
}
