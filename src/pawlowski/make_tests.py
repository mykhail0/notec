import subprocess

def make_tests(size, seed):
    ret = subprocess.run(["./test"], input=f"{size} {seed}".encode(), capture_output=True)
    meta = [x.partition(': ') for x in ret.stderr.decode().split('\n')]
    meta = {x[0]: x[2] for x in meta}

    print(f"{size};{seed};{ret.stdout.decode()}")
    # print(meta['Test'] + ';' + ret.stdout.decode())
    # print(meta['Number of characters actually used in the test'].partition('/')[0])


# for i in range(1000):
#     make_tests(8, i)
# for i in range(1000):
#     make_tests(16, 1000+i)
# for i in range(1000):
#     make_tests(32, 2000+i)
# for i in range(1000):
#     make_tests(48, 3000+i)
# for i in range(1000):
#     make_tests(64, 4000+i)

# for i in range(1000):
#     make_tests(1024, 5000+i)
# for i in range(1000):
#     make_tests(4*1024, 6000+i)
# for i in range(1000):
#     make_tests(8*1024, 7000+i)
# for i in range(1000):
#     make_tests(16*1024, 8000+i)
# for i in range(1000):
#     make_tests(32*1024, 9000+i)

# for i in range(50):
#     make_tests(1024*1024, 10000+i)
# for i in range(20):
#     make_tests(4*1024*1024, 10050+i)
# for i in range(10):
#     make_tests(8*1024*1024, 10070+i)
# for i in range(10):
#     make_tests(16*1024*1024, 10080+i)
# for i in range(10):
#     make_tests(32*1024*1024, 10090+i)

for i in range(10):
    make_tests(1024*1024*1024, 10100+i)