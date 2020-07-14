#!/bin/bash

## build docker image in the PARENT DIRECTORY
docker build ../ -t setbench -f Dockerfile
