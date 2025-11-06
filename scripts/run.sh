#!/bin/bash
end=$1
for i in $(seq 1 "$end"); do
  make "$i"
done
