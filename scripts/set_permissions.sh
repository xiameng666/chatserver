#!/bin/bash

# 设置所有可执行文件的权限
find ./bin -type f -exec chmod +x {} \;

# 或者只针对特定文件
chmod +x ./bin/ChatServer 