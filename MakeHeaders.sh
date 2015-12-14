#!/bin/sh
rm -rf include/*
mkdir include/lcp
mkdir include/lcp/apple
cp src/**/public/*.h include/lcp/
cp platform/apple/**/*.h include/lcp/apple/
