#!/bin/bash

SHADERS=$(find ./shaders -type f \( -name "*.vert" -or -name "*.frag" \))

for shader in $SHADERS; do
  glslc "$shader" -o "$shader.spv"
done

