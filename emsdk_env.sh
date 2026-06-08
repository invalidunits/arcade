# 1. Create a build directory
$(mkdir build-ems && cd build-ems) >> /dev/null

# 2. Configure with emcmake
emcmake cmake  -G 'Ninja' ..
