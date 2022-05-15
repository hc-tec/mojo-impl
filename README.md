# mojo-impl

This is a project for learn, cause I plan to implement a mini Mojo.

Mojo is used as IPC lib in chromium([mojo_and_services](https://chromium.googlesource.com/chromium/src/+/HEAD/docs/mojo_and_services.md)).
 
I am interested in Mojo, so I create this project.

If you want to run this project, you should add libs of third_party into /usr/local/lib.
And I prepare a shell for you, so you can just run install.sh in third_party to install them.

## build
```c++
git clone https://github.com/hc-tec/mojo-impl.git
cd mojo-impl

cd third_party
sudo sh install.sh
cd ..

mkdir build && cd build
cmake ..
make
```

## run test
```c++
cd bin
./core
```

## Some articles about this project
[Mojo IPC 文件描述符的传递](https://zhuanlan.zhihu.com/p/510935402)

[Mojo IPC 设计与Invitation机制实现](https://zhuanlan.zhihu.com/p/510935402)
