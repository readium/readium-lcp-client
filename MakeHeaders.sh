#!/bin/sh
rm -rf include/*
mkdir include/lcp
mkdir include/lcp/apple
cp src/**/Public/*.h include/lcp/
cp platform/apple/**/*.h include/lcp/apple/
