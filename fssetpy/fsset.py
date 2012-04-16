import socket

class Connection(object):
    def __init__(self, host, port):
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
        self._socket.connect((host, port))

    def add(self, set_name, key_name):
        self._demand('SET/%s/%s\n' % (set_name, key_name), 'ADDED')

    def check(self, set_name, key_name):
        return self._ask('GET/%s/%s\n' % (set_name, key_name),
            'HASKEY', 'NOKEY')

    def clear(self, set_name, key_name):
        self._demand('DEL/%s/%s\n' % (set_name, key_name), 'CLEARED')

    def close(self):
        self._socket.close()

    def _ask(self, send, expect_yes, expect_no):
        self._socket.sendall(send)
        rs = self._socket.recv(16).strip()
        print rs
        if rs == expect_yes:
            return True
        elif rs == expect_no:
            return False
        else:
            # raise an assertion error due to invalid response
            assert(rs == expect_yes or rs == expect_no)

    def _demand(self, send, expect):
        assert(self._ask(send, expect) == True)
