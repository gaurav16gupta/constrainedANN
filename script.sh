# for i in {100, 150, 200, 500, 1000, 2000}; do taskset -c 0 ./query 128 1000 "$i" | sed -n 9p; done
# for i in {100, 150, 200, 500, 1000, 2000}; do taskset -c 0 ./query 255 1000 "$i" | sed -n 9p; done
# for i in {100, 150, 200, 500, 1000, 2000}; do taskset -c 0 ./query 512 1000 "$i" | sed -n 9p; done
# for i in {100, 150, 200, 500, 1000, 2000}; do taskset -c 0 ./query 1024 1000 "$i" | sed -n 9p; done
# for i in {100, 150, 200, 500, 1000, 2000}; do taskset -c 0 ./query 2048 1000 "$i" | sed -n 9p; done
# for i in { 100, 150, 200, 500, 1000, 2000}; do taskset -c 0 ./query 4096 1000 "$i" | sed -n 9p; done
# for i in { 100, 150, 200, 500, 1000, 2000}; do taskset -c 0 ./query 8192 1000 "$i" | sed -n 9p; done
# for i in { 100, 150, 200, 500, 1000, 2000}; do taskset -c 0 ./query 16384 1000 "$i" | sed -n 9p; done
# for i in { 128, 256, 512, 1024, 2048, 4096, 8192, 16384}; do ./index "$i"; done
for i in { 50, 70, 100, 150, 200, 500, 1000, 5000, 10000, 100000, 900000, 999999}; do taskset -c 0 ./query sift 1024 10 "$i" | sed -n 6p; done

