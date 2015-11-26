#!/bin/sh
rm -rf include/lcp/*
cp src/**/Public/*.h include/lcp/
cp platform/apple/**/*.h include/lcp/
