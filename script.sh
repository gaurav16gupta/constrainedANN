# for i in {100, 150, 200, 500, 1000, 2000}; do taskset -c 0 ./query 128 1000 "$i" | sed -n 9p; done
# for i in {100, 150, 200, 500, 1000, 2000}; do taskset -c 0 ./query 255 1000 "$i" | sed -n 9p; done
# for i in {100, 150, 200, 500, 1000, 2000}; do taskset -c 0 ./query 512 1000 "$i" | sed -n 9p; done
# for i in {100, 150, 200, 500, 1000, 2000}; do taskset -c 0 ./query 1024 1000 "$i" | sed -n 9p; done
# for i in {100, 150, 200, 500, 1000, 2000}; do taskset -c 0 ./query 2048 1000 "$i" | sed -n 9p; done
# for i in { 100, 150, 200, 500, 1000, 2000}; do taskset -c 0 ./query 4096 1000 "$i" | sed -n 9p; done
# for i in { 100, 150, 200, 500, 1000, 2000}; do taskset -c 0 ./query 8192 1000 "$i" | sed -n 9p; done
# for i in { 100, 150, 200, 500, 1000, 2000}; do taskset -c 0 ./query 16384 1000 "$i" | sed -n 9p; done
# for i in { 128, 256, 512, 1024, 2048, 4096, 8192, 16384}; do ./index "$i"; done
# echo "Mode1"
# for i in 100 200 300 500 700 1000 1500 2000 3000; do taskset -c 0 ./query sift 1024 3 "$i" 1 | sed -n 7p; done
# echo "Mode2"
# for i in 100 200 300 500 700 1000 1500 2000 3000; do taskset -c 0 ./query sift 1024 3 "$i" 2 | sed -n 7p; done
# echo "Mode3"
# for i in 100 200 300 500 700 1000 1500 2000 3000; do taskset -c 0 ./query sift 1024 3 "$i" 3 | sed -n 7p; done


# make index
# taskset -c 0-31 ./index /scratch/gg29/data/sift/base.fvecs \
#         /scratch/gg29/data/sift/label_base_3.txt \
#         indices/sift1024kmeansMode1 \
#         1024 kmeans 1

# taskset -c 0-31 ./index /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         indices/glove-100-1024kmeansMode1 \
#         1024 kmeans 1

# taskset -c 0-31 ./index /scratch/gg29/data/gist/base.fvecs \
#         /scratch/gg29/data/gist/label_base_3.txt \
#         indices/gist1024kmeansMode1 \
#         1024 kmeans 1

# taskset -c 0-31 ./index /scratch/gg29/data/crawl/base.fvecs \
#         /scratch/gg29/data/crawl/label_base_3.txt \
#         indices/crawl2048kmeansMode1 \
#         2048 kmeans 1

# taskset -c 0-31 ./index /scratch/gg29/data/audio/base.fvecs \
#         /scratch/gg29/data/audio/label_base_3.txt \
#         indices/audio256kmeansMode1 \
#         256 kmeans 1

# taskset -c 0-31 ./index /scratch/gg29/data/msong/base.fvecs \
#         /scratch/gg29/data/msong/label_base_3.txt \
#         indices/msong1024kmeansMode1 \
#         1024 kmeans 1


# python3 include/bliss/dataPrepare_constrained.py --data="/scratch/gg29/data/glove-100"
# python3 include/bliss/dataPrepare_constrained.py --data="/scratch/gg29/data/audio"
# python3 include/bliss/dataPrepare_constrained.py --data="/scratch/gg29/data/msong"


# python3 include/bliss/construct.py --index='glove-100_epc40_K100_B1024_R1' --hdim=256 --mode=1 --kn=100
# python3 include/bliss/construct.py --index='glove-100_epc40_K100_B1024_R1' --hdim=256 --mode=2 --kn=100

# python3 include/bliss/construct.py --index='gist_epc40_K25_B1024_R1' --hdim=256 --mode=1 --kn=25
# python3 include/bliss/construct.py --index='gist_epc40_K60_B1024_R1' --hdim=256 --mode=2 --kn=60

# python3 include/bliss/construct.py --index='crawl_epc40_K10_B2048_R1' --hdim=256 --mode=1 --kn=10
# python3 include/bliss/construct.py --index='crawl_epc40_K15_B2048_R1' --hdim=256 --mode=2 --kn=15

# python3 include/bliss/construct.py --index='audio_epc40_K4_B256_R1' --hdim=256 --mode=1 --kn=4
# python3 include/bliss/construct.py --index='audio_epc40_K50_B256_R1' --hdim=256 --mode=2 --kn=50

# python3 include/bliss/construct.py --index='msong_epc40_K3_B1024_R1' --hdim=256 --mode=1 --kn=3
# python3 include/bliss/construct.py --index='msong_epc40_K100_B1024_R1' --hdim=256 --mode=2 --kn=100


# make index
# taskset -c 0-31 ./index /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         indices/glove-100blissMode1 \
#         1024 bliss 1

# taskset -c 0-31 ./index /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         indices/glove-100blissMode2 \
#         1024 bliss 2

# taskset -c 0-31 ./index /scratch/gg29/data/gist/base.fvecs \
#         /scratch/gg29/data/gist/label_base_3.txt \
#         indices/gistblissMode1 \
#         1024 bliss 1

# taskset -c 0-31 ./index /scratch/gg29/data/gist/base.fvecs \
#         /scratch/gg29/data/gist/label_base_3.txt \
#         indices/gistblissMode2 \
#         1024 bliss 2

# taskset -c 0-31 ./index /scratch/gg29/data/crawl/base.fvecs \
#         /scratch/gg29/data/crawl/label_base_3.txt \
#         indices/crawlblissMode1 \
#         2048 bliss 1

# taskset -c 0-31 ./index /scratch/gg29/data/crawl/base.fvecs \
#         /scratch/gg29/data/crawl/label_base_3.txt \
#         indices/crawlblissMode2 \
#         2048 bliss 2

# taskset -c 0-31 ./index /scratch/gg29/data/audio/base.fvecs \
#         /scratch/gg29/data/audio/label_base_3.txt \
#         indices/audioblissMode1 \
#         256 bliss 1

# taskset -c 0-31 ./index /scratch/gg29/data/audio/base.fvecs \
#         /scratch/gg29/data/audio/label_base_3.txt \
#         indices/audioblissMode2 \
#         256 bliss 2

# taskset -c 0-31 ./index /scratch/gg29/data/msong/base.fvecs \
#         /scratch/gg29/data/msong/label_base_3.txt \
#         indices/msongblissMode1 \
#         1024 bliss 1

# taskset -c 0-31 ./index /scratch/gg29/data/msong/base.fvecs \
#         /scratch/gg29/data/msong/label_base_3.txt \
#         indices/msongblissMode2 \
#         1024 bliss 2

# make query

#  taskset -c 0 ./query /scratch/gg29/data/sift/base.fvecs \
#         /scratch/gg29/data/sift/label_base_3.txt \
#         /scratch/gg29/data/sift/query.fvecs \
#         /scratch/gg29/data/sift/label_query_3.txt \
#         indices/sift1024kmeansMode1 \
#         /scratch/gg29/data/sift/label_3_hard_groundtruth.ivecs \
#         1024 kmeans 1 1000000

taskset -c 0 ./query /scratch/gg29/data/glove-100/base.fvecs \
        /scratch/gg29/data/glove-100/label_base_3.txt \
        /scratch/gg29/data/glove-100/query.fvecs \
        /scratch/gg29/data/glove-100/label_query_3.txt \
        indices/glove-100-1024kmeansMode1 \
        /scratch/gg29/data/glove-100/label_3_hard_groundtruth.ivecs \
        1024 kmeans 1 1183514

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/sift/base.fvecs \
#         /scratch/gg29/data/sift/label_base_3.txt \
#         /scratch/gg29/data/sift/query.fvecs \
#         /scratch/gg29/data/sift/label_query_3.txt \
#         indices/sift1024kmeansMode1 \
#         /scratch/gg29/data/sift/label_3_hard_groundtruth.ivecs \
#         1024 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_sift.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/sift/base.fvecs \
#         /scratch/gg29/data/sift/label_base_3.txt \
#         /scratch/gg29/data/sift/query.fvecs \
#         /scratch/gg29/data/sift/label_query_3.txt \
#         indices/sift1024blissMode1 \
#         /scratch/gg29/data/sift/label_3_hard_groundtruth.ivecs \
#         1024 bliss 1 "$i"| sed -n 7p; done > results2/CAPSbliss1_sift.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/sift/base.fvecs \
#         /scratch/gg29/data/sift/label_base_3.txt \
#         /scratch/gg29/data/sift/query.fvecs \
#         /scratch/gg29/data/sift/label_query_3.txt \
#         indices/sift1024blissMode2 \
#         /scratch/gg29/data/sift/label_3_hard_groundtruth.ivecs \
#         1024 bliss 2 "$i"| sed -n 7p; done > results2/CAPSbliss2_sift.txt


# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000 500000 999999 ; do taskset -c 0 \
# ./query /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         /scratch/gg29/data/glove-100/query.fvecs \
#         /scratch/gg29/data/glove-100/label_query_3.txt \
#         indices/glove-100-1024kmeansMode1 \
#         /scratch/gg29/data/glove-100/label_3_hard_groundtruth.ivecs \
#         1024 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_glove-100.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000 500000 999999; do taskset -c 0 \
# ./query /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         /scratch/gg29/data/glove-100/query.fvecs \
#         /scratch/gg29/data/glove-100/label_query_3.txt \
#         indices/glove-100blissMode1 \
#         /scratch/gg29/data/glove-100/label_3_hard_groundtruth.ivecs \
#         1024 bliss 1 "$i"| sed -n 7p; done > results2/CAPSbliss1_glove-100.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000 500000 999999; do taskset -c 0 \
# ./query /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         /scratch/gg29/data/glove-100/query.fvecs \
#         /scratch/gg29/data/glove-100/label_query_3.txt \
#         indices/glove-100blissMode2 \
#         /scratch/gg29/data/glove-100/label_3_hard_groundtruth.ivecs \
#         1024 bliss 2 "$i"| sed -n 7p; done > results2/CAPSbliss2_glove-100.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/gist/base.fvecs \
#         /scratch/gg29/data/gist/label_base_3.txt \
#         /scratch/gg29/data/gist/query.fvecs \
#         /scratch/gg29/data/gist/label_query_3.txt \
#         indices/gist1024kmeansMode1 \
#         /scratch/gg29/data/gist/label_3_hard_groundtruth.ivecs \
#         1024 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_gist.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/gist/base.fvecs \
#         /scratch/gg29/data/gist/label_base_3.txt \
#         /scratch/gg29/data/gist/query.fvecs \
#         /scratch/gg29/data/gist/label_query_3.txt \
#         indices/gistblissMode1 \
#         /scratch/gg29/data/gist/label_3_hard_groundtruth.ivecs \
#         1024 bliss 1 "$i"| sed -n 7p; done > results2/CAPSbliss1_gist.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/gist/base.fvecs \
#         /scratch/gg29/data/gist/label_base_3.txt \
#         /scratch/gg29/data/gist/query.fvecs \
#         /scratch/gg29/data/gist/label_query_3.txt \
#         indices/gistblissMode2 \
#         /scratch/gg29/data/gist/label_3_hard_groundtruth.ivecs \
#         1024 bliss 2 "$i"| sed -n 7p; done > results2/CAPSbliss2_gist.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/crawl/base.fvecs \
#         /scratch/gg29/data/crawl/label_base_3.txt \
#         /scratch/gg29/data/crawl/query.fvecs \
#         /scratch/gg29/data/crawl/label_query_3.txt \
#         indices/crawl2048kmeansMode1 \
#         /scratch/gg29/data/crawl/label_3_hard_groundtruth.ivecs \
#         2048 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_crawl.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/crawl/base.fvecs \
#         /scratch/gg29/data/crawl/label_base_3.txt \
#         /scratch/gg29/data/crawl/query.fvecs \
#         /scratch/gg29/data/crawl/label_query_3.txt \
#         indices/crawlblissMode1 \
#         /scratch/gg29/data/crawl/label_3_hard_groundtruth.ivecs \
#         2048 bliss 1 "$i"| sed -n 7p; done > results2/CAPSbliss1_crawl.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/crawl/base.fvecs \
#         /scratch/gg29/data/crawl/label_base_3.txt \
#         /scratch/gg29/data/crawl/query.fvecs \
#         /scratch/gg29/data/crawl/label_query_3.txt \
#         indices/crawlblissMode2 \
#         /scratch/gg29/data/crawl/label_3_hard_groundtruth.ivecs \
#         2048 bliss 2 "$i"| sed -n 7p; done > results2/CAPSbliss2_crawl.txt


# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/audio/base.fvecs \
#         /scratch/gg29/data/audio/label_base_3.txt \
#         /scratch/gg29/data/audio/query.fvecs \
#         /scratch/gg29/data/audio/label_query_3.txt \
#         indices/audio256kmeansMode1 \
#         /scratch/gg29/data/audio/label_3_hard_groundtruth.ivecs \
#         256 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_audio.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/audio/base.fvecs \
#         /scratch/gg29/data/audio/label_base_3.txt \
#         /scratch/gg29/data/audio/query.fvecs \
#         /scratch/gg29/data/audio/label_query_3.txt \
#         indices/audioblissMode1 \
#         /scratch/gg29/data/audio/label_3_hard_groundtruth.ivecs \
#         256 bliss 1 "$i"| sed -n 7p; done > results2/CAPSbliss1_audio.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/audio/base.fvecs \
#         /scratch/gg29/data/audio/label_base_3.txt \
#         /scratch/gg29/data/audio/query.fvecs \
#         /scratch/gg29/data/audio/label_query_3.txt \
#         indices/audioblissMode2 \
#         /scratch/gg29/data/audio/label_3_hard_groundtruth.ivecs \
#         256 bliss 2 "$i"| sed -n 7p; done > results2/CAPSbliss2_audio.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/msong/base.fvecs \
#         /scratch/gg29/data/msong/label_base_3.txt \
#         /scratch/gg29/data/msong/query.fvecs \
#         /scratch/gg29/data/msong/label_query_3.txt \
#         indices/msong1024kmeansMode1 \
#         /scratch/gg29/data/msong/label_3_hard_groundtruth.ivecs \
#         1024 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_msong.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/msong/base.fvecs \
#         /scratch/gg29/data/msong/label_base_3.txt \
#         /scratch/gg29/data/msong/query.fvecs \
#         /scratch/gg29/data/msong/label_query_3.txt \
#         indices/msongblissMode1 \
#         /scratch/gg29/data/msong/label_3_hard_groundtruth.ivecs \
#         1024 bliss 1 "$i"| sed -n 7p; done > results2/CAPSbliss1_msong.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/msong/base.fvecs \
#         /scratch/gg29/data/msong/label_base_3.txt \
#         /scratch/gg29/data/msong/query.fvecs \
#         /scratch/gg29/data/msong/label_query_3.txt \
#         indices/msongblissMode2 \
#         /scratch/gg29/data/msong/label_3_hard_groundtruth.ivecs \
#         1024 bliss 2 "$i"| sed -n 7p; done > results2/CAPSbliss2_msong.txt
