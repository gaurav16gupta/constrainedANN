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
# taskset -c 0-63 ./index /scratch/gg29/data/sift/base.fvecs \
#         /scratch/gg29/data/sift/label_base_3.txt \
#         indices/sift1024kmeansMode1 \
#         1024 kmeans 1

# taskset -c 0-63 ./index /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         indices/glove-100-1024kmeansMode1 \
#         1024 kmeans 1

# taskset -c 0-63 ./index /scratch/gg29/data/gist/base.fvecs \
#         /scratch/gg29/data/gist/label_base_3.txt \
#         indices/gist1024kmeansMode1 \
#         1024 kmeans 1

# taskset -c 0-63 ./index /scratch/gg29/data/crawl/base.fvecs \
#         /scratch/gg29/data/crawl/label_base_3.txt \
#         indices/crawl2048kmeansMode1 \
#         2048 kmeans 1

# taskset -c 0-63 ./index /scratch/gg29/data/audio/base.fvecs \
#         /scratch/gg29/data/audio/label_base_3.txt \
#         indices/audio256kmeansMode1 \
#         256 kmeans 1

# taskset -c 0-63 ./index /scratch/gg29/data/msong/base.fvecs \
#         /scratch/gg29/data/msong/label_base_3.txt \
#         indices/msong1024kmeansMode1 \
#         1024 kmeans 1

# taskset -c 64-253 ython3 include/bliss/dataPrepare_constrained.py --data="/scratch/gg29/data/sift"

# taskset -c 64-253 python3 include/bliss/dataPrepare_constrained.py --data="/scratch/gg29/data/glove-100"
# taskset -c 64-253 python3 include/bliss/dataPrepare_constrained.py --data="/scratch/gg29/data/audio"
# taskset -c 64-253 python3 include/bliss/dataPrepare_constrained.py --data="/scratch/gg29/data/msong"

# python3 include/bliss/construct.py --index='sift_epc40_K4_B1024_R1' --hdim=256 --mode=1 --kn=4
# python3 include/bliss/construct.py --index='sift_epc40_K10_B1024_R1' --hdim=256 --mode=2 --kn=10

# python3 include/bliss/construct.py --index='glove-100_epc40_K5_B1024_R1' --hdim=256 --mode=1 --kn=5
# python3 include/bliss/construct.py --index='glove-100_epc40_K20_B1024_R1' --hdim=256 --mode=2 --kn=20
# python3 include/bliss/construct.py --index='glove-100_epc40_K20_B1024_R1' --hdim=256 --mode=3 --kn=20

# python3 include/bliss/construct.py --index='gist_epc40_K25_B1024_R1' --hdim=256 --mode=1 --kn=25
# python3 include/bliss/construct.py --index='gist_epc40_K45_B1024_R1' --hdim=256 --mode=2 --kn=45

# python3 include/bliss/construct.py --index='crawl_epc40_K10_B2048_R1' --hdim=256 --mode=1 --kn=10
# python3 include/bliss/construct.py --index='crawl_epc40_K15_B2048_R1' --hdim=256 --mode=2 --kn=15

# python3 include/bliss/construct.py --index='audio_epc40_K4_B256_R1' --hdim=256 --mode=1 --kn=4
# python3 include/bliss/construct.py --index='audio_epc40_K70_B256_R1' --hdim=256 --mode=2 --kn=70

# python3 include/bliss/construct.py --index='msong_epc40_K3_B1024_R1' --hdim=256 --mode=1 --kn=3
# python3 include/bliss/construct.py --index='msong_epc40_K70_B1024_R1' --hdim=256 --mode=2 --kn=70



# make index
# taskset -c 0-63 ./index /scratch/gg29/data/sift/base.fvecs \
#         /scratch/gg29/data/sift/label_base_3.txt \
#         indices/siftblissMode1 \
#         1024 bliss 1

# taskset -c 0-63 ./index /scratch/gg29/data/sift/base.fvecs \
#         /scratch/gg29/data/sift/label_base_3.txt \
#         indices/siftblissMode2 \
#         1024 bliss 2

# taskset -c 0-63 ./index /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         indices/glove-100blissMode1 \
#         1024 bliss 1

# taskset -c 0-63 ./index /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         indices/glove-100blissMode2 \
#         1024 bliss 2

# taskset -c 0-63 ./index /scratch/gg29/data/gist/base.fvecs \
#         /scratch/gg29/data/gist/label_base_3.txt \
#         indices/gistblissMode1 \
#         1024 bliss 1

# taskset -c 0-63 ./index /scratch/gg29/data/gist/base.fvecs \
#         /scratch/gg29/data/gist/label_base_3.txt \
#         indices/gistblissMode2 \
#         1024 bliss 2

# taskset -c 0-63 ./index /scratch/gg29/data/crawl/base.fvecs \
#         /scratch/gg29/data/crawl/label_base_3.txt \
#         indices/crawlblissMode1 \
#         2048 bliss 1

# taskset -c 0-63 ./index /scratch/gg29/data/crawl/base.fvecs \
#         /scratch/gg29/data/crawl/label_base_3.txt \
#         indices/crawlblissMode2 \
#         2048 bliss 2

# taskset -c 0-63 ./index /scratch/gg29/data/audio/base.fvecs \
#         /scratch/gg29/data/audio/label_base_3.txt \
#         indices/audioblissMode1 \
#         256 bliss 1

# taskset -c 0-63 ./index /scratch/gg29/data/audio/base.fvecs \
#         /scratch/gg29/data/audio/label_base_3.txt \
#         indices/audioblissMode2 \
#         256 bliss 2

# taskset -c 0-63 ./index /scratch/gg29/data/msong/base.fvecs \
#         /scratch/gg29/data/msong/label_base_3.txt \
#         indices/msongblissMode1 \
#         1024 bliss 1

# taskset -c 0-63 ./index /scratch/gg29/data/msong/base.fvecs \
#         /scratch/gg29/data/msong/label_base_3.txt \
#         indices/msongblissMode2 \
#         1024 bliss 2

# make query
# for i in 1 2 3 4 5 6 7 8 10 12 14 18 25; do \
# taskset -c 0 ./query /scratch/gg29/data/sift/base.fvecs \
#         /scratch/gg29/data/sift/label_base_3.txt \
#         /scratch/gg29/data/sift/query.fvecs \
#         /scratch/gg29/data/sift/label_query_3.txt \
#         indices/sift1024kmeansMode1 \
#         /scratch/gg29/data/sift/label_3_hard_groundtruth.ivecs \
#         1024 kmeans 1 500 | sed -n 7p; done

# for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20; do \
# taskset -c 0 ./query /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         /scratch/gg29/data/glove-100/query.fvecs \
#         /scratch/gg29/data/glove-100/label_query_3.txt \
#         indices/glove-100-1024kmeansMode1 \
#         /scratch/gg29/data/glove-100/label_3_hard_groundtruth.ivecs \
#         1024 kmeans 1 5000 | sed -n 7p; done

# for i in 1 2 3 4 5 6 7 8 9 10 12 14 16 20 25 30 40 50 70 100; do \
# taskset -c 0 echo "$i" | ./query /scratch/gg29/data/gist/base.fvecs \
#         /scratch/gg29/data/gist/label_base_3.txt \
#         /scratch/gg29/data/gist/query.fvecs \
#         /scratch/gg29/data/gist/label_query_3.txt \
#         indices/gist1024kmeansMode1 \
#         /scratch/gg29/data/gist/label_3_hard_groundtruth.ivecs \
#         1024 kmeans 1 8000 | sed -n 7p; done

# for i in 1 2 3 4 5 6 7 8 9 10 12 14 16 20 25 30 40 50 70 100; do \
# taskset -c 0 echo "$i" | ./query /scratch/gg29/data/crawl/base.fvecs \
#         /scratch/gg29/data/crawl/label_base_3.txt \
#         /scratch/gg29/data/crawl/query.fvecs \
#         /scratch/gg29/data/crawl/label_query_3.txt \
#         indices/crawl2048kmeansMode1 \
#         /scratch/gg29/data/crawl/label_3_hard_groundtruth.ivecs \
#         2048 kmeans 1 4000 | sed -n 7p; done

# for i in 1 2 3 4 5 6 7 8 9 10 12 14 16 20 25 30 40 50 70 100; do \
# taskset -c 0 echo "$i" | ./query /scratch/gg29/data/msong/base.fvecs \
#         /scratch/gg29/data/msong/label_base_3.txt \
#         /scratch/gg29/data/msong/query.fvecs \
#         /scratch/gg29/data/msong/label_query_3.txt \
#         indices/msong1024kmeansMode1 \
#         /scratch/gg29/data/msong/label_3_hard_groundtruth.ivecs \
#         1024 kmeans 1 200 | sed -n 7p; done

# for i in 1 2 3 4 5 6 7 8 9 10 12 14 16 20 25 30 40 50 70 100; do \
# taskset -c 0 echo "$i" | ./query /scratch/gg29/data/audio/base.fvecs \
#         /scratch/gg29/data/audio/label_base_3.txt \
#         /scratch/gg29/data/audio/query.fvecs \
#         /scratch/gg29/data/audio/label_query_3.txt \
#         indices/audio256kmeansMode1 \
#         /scratch/gg29/data/audio/label_3_hard_groundtruth.ivecs \
#         256 kmeans 1 200 | sed -n 7p; done



# make query
# for i in 50 100 200 300 500 700 1000 1500 2000 3000 4000 5000 8000; do taskset -c 0 \
# ./query /scratch/gg29/data/sift/base.fvecs \
#         /scratch/gg29/data/sift/label_base_3.txt \
#         /scratch/gg29/data/sift/query.fvecs \
#         /scratch/gg29/data/sift/label_query_3.txt \
#         indices/sift1024kmeansMode1 \
#         /scratch/gg29/data/sift/label_3_hard_groundtruth.ivecs \
#         1024 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_siftNew.txt

# for i in 50 100 200 300 500 700 1000 1500 2000 3000 4000 5000 8000; do taskset -c 0 \
# ./query /scratch/gg29/data/sift/base.fvecs \
#         /scratch/gg29/data/sift/label_base_3.txt \
#         /scratch/gg29/data/sift/query.fvecs \
#         /scratch/gg29/data/sift/label_query_3.txt \
#         indices/siftblissMode1 \
#         /scratch/gg29/data/sift/label_3_hard_groundtruth.ivecs \
#         1024 bliss 1 "$i"| sed -n 7p; done > results2/CAPSbliss1_siftNew.txt

# for i in 50 100 200 300 500 700 1000 1500 2000 3000 4000 5000 8000; do taskset -c 0 \
# ./query /scratch/gg29/data/sift/base.fvecs \
#         /scratch/gg29/data/sift/label_base_3.txt \
#         /scratch/gg29/data/sift/query.fvecs \
#         /scratch/gg29/data/sift/label_query_3.txt \
#         indices/siftblissMode2 \
#         /scratch/gg29/data/sift/label_3_hard_groundtruth.ivecs \
#         1024 bliss 2 "$i"| sed -n 7p; done > results2/CAPSbliss2_siftNew.txt


# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         /scratch/gg29/data/glove-100/query.fvecs \
#         /scratch/gg29/data/glove-100/label_query_3.txt \
#         indices/glove-100-1024kmeansMode1 \
#         /scratch/gg29/data/glove-100/label_3_hard_groundtruth.ivecs \
#         1024 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_glove-100New.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         /scratch/gg29/data/glove-100/query.fvecs \
#         /scratch/gg29/data/glove-100/label_query_3.txt \
#         indices/glove-100blissMode1 \
#         /scratch/gg29/data/glove-100/label_3_hard_groundtruth.ivecs \
#         1024 bliss 1 "$i"| sed -n 7p; done > results2/CAPSbliss1_glove-100New.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         /scratch/gg29/data/glove-100/query.fvecs \
#         /scratch/gg29/data/glove-100/label_query_3.txt \
#         indices/glove-100blissMode2 \
#         /scratch/gg29/data/glove-100/label_3_hard_groundtruth.ivecs \
#         1024 bliss 2 "$i"| sed -n 7p; done > results2/CAPSbliss2_glove-100New.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/gist/base.fvecs \
#         /scratch/gg29/data/gist/label_base_3.txt \
#         /scratch/gg29/data/gist/query.fvecs \
#         /scratch/gg29/data/gist/label_query_3.txt \
#         indices/gist1024kmeansMode1 \
#         /scratch/gg29/data/gist/label_3_hard_groundtruth.ivecs \
#         1024 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_gistNew.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/gist/base.fvecs \
#         /scratch/gg29/data/gist/label_base_3.txt \
#         /scratch/gg29/data/gist/query.fvecs \
#         /scratch/gg29/data/gist/label_query_3.txt \
#         indices/gistblissMode1 \
#         /scratch/gg29/data/gist/label_3_hard_groundtruth.ivecs \
#         1024 bliss 1 "$i"| sed -n 7p; done > results2/CAPSbliss1_gistNew.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/gist/base.fvecs \
#         /scratch/gg29/data/gist/label_base_3.txt \
#         /scratch/gg29/data/gist/query.fvecs \
#         /scratch/gg29/data/gist/label_query_3.txt \
#         indices/gistblissMode2 \
#         /scratch/gg29/data/gist/label_3_hard_groundtruth.ivecs \
#         1024 bliss 2 "$i"| sed -n 7p; done > results2/CAPSbliss2_gistNew.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/crawl/base.fvecs \
#         /scratch/gg29/data/crawl/label_base_3.txt \
#         /scratch/gg29/data/crawl/query.fvecs \
#         /scratch/gg29/data/crawl/label_query_3.txt \
#         indices/crawl2048kmeansMode1 \
#         /scratch/gg29/data/crawl/label_3_hard_groundtruth.ivecs \
#         2048 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_crawlNew.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/crawl/base.fvecs \
#         /scratch/gg29/data/crawl/label_base_3.txt \
#         /scratch/gg29/data/crawl/query.fvecs \
#         /scratch/gg29/data/crawl/label_query_3.txt \
#         indices/crawlblissMode1 \
#         /scratch/gg29/data/crawl/label_3_hard_groundtruth.ivecs \
#         2048 bliss 1 "$i"| sed -n 7p; done > results2/CAPSbliss1_crawlNew.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 50000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/crawl/base.fvecs \
#         /scratch/gg29/data/crawl/label_base_3.txt \
#         /scratch/gg29/data/crawl/query.fvecs \
#         /scratch/gg29/data/crawl/label_query_3.txt \
#         indices/crawlblissMode2 \
#         /scratch/gg29/data/crawl/label_3_hard_groundtruth.ivecs \
#         2048 bliss 2 "$i"| sed -n 7p; done > results2/CAPSbliss2_crawlNew.txt


# for i in 10 50 100 200 500 700 1000 1500 2000 3000 4000 5000 8000 10000 50000; do taskset -c 0 \
# ./query /scratch/gg29/data/audio/base.fvecs \
#         /scratch/gg29/data/audio/label_base_3.txt \
#         /scratch/gg29/data/audio/query.fvecs \
#         /scratch/gg29/data/audio/label_query_3.txt \
#         indices/audio256kmeansMode1 \
#         /scratch/gg29/data/audio/label_3_hard_groundtruth.ivecs \
#         256 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_audioNew.txt

# for i in 10 50 100 200 500 700 1000 1500 2000 3000 4000 5000 8000 10000 50000; do taskset -c 0 \
# ./query /scratch/gg29/data/audio/base.fvecs \
#         /scratch/gg29/data/audio/label_base_3.txt \
#         /scratch/gg29/data/audio/query.fvecs \
#         /scratch/gg29/data/audio/label_query_3.txt \
#         indices/audioblissMode1 \
#         /scratch/gg29/data/audio/label_3_hard_groundtruth.ivecs \
#         256 bliss 1 "$i"| sed -n 7p; done > results2/CAPSbliss1_audioNew.txt

# for i in 10 50 100 200 500 700 1000 1500 2000 3000 4000 5000 8000 10000 50000; do taskset -c 0 \
# ./query /scratch/gg29/data/audio/base.fvecs \
#         /scratch/gg29/data/audio/label_base_3.txt \
#         /scratch/gg29/data/audio/query.fvecs \
#         /scratch/gg29/data/audio/label_query_3.txt \
#         indices/audioblissMode2 \
#         /scratch/gg29/data/audio/label_3_hard_groundtruth.ivecs \
#         256 bliss 2 "$i"| sed -n 7p; done > results2/CAPSbliss2_audioNew.txt

# for i in 50 100 200 300 500 700 1000 1500 2000; do taskset -c 0 \
# ./query /scratch/gg29/data/msong/base.fvecs \
#         /scratch/gg29/data/msong/label_base_3.txt \
#         /scratch/gg29/data/msong/query.fvecs \
#         /scratch/gg29/data/msong/label_query_3.txt \
#         indices/msong1024kmeansMode1 \
#         /scratch/gg29/data/msong/label_3_hard_groundtruth.ivecs \
#         1024 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_msongNew.txt

# for i in 50 100 200 300 500 700 1000 1500 2000 3000; do taskset -c 0 \
# ./query /scratch/gg29/data/msong/base.fvecs \
#         /scratch/gg29/data/msong/label_base_3.txt \
#         /scratch/gg29/data/msong/query.fvecs \
#         /scratch/gg29/data/msong/label_query_3.txt \
#         indices/msongblissMode1 \
#         /scratch/gg29/data/msong/label_3_hard_groundtruth.ivecs \
#         1024 bliss 1 "$i"| sed -n 7p; done > results2/CAPSbliss1_msongNew.txt

# for i in 50 100 200 300 500 700 1000 1500 2000 3000; do taskset -c 0 \
# ./query /scratch/gg29/data/msong/base.fvecs \
#         /scratch/gg29/data/msong/label_base_3.txt \
#         /scratch/gg29/data/msong/query.fvecs \
#         /scratch/gg29/data/msong/label_query_3.txt \
#         indices/msongblissMode2 \
#         /scratch/gg29/data/msong/label_3_hard_groundtruth.ivecs \
#         1024 bliss 2 "$i"| sed -n 7p; done > results2/CAPSbliss2_msongNew.txt


# taskset -c 64-253 python3 include/bliss/dataPrepare_constrained.py --data="/scratch/gg29/data/gist"
# taskset -c 64-253 python3 include/bliss/dataPrepare_constrained.py --data="/scratch/gg29/data/crawl"

# python3 include/bliss/construct.py --index='gist_epc40_K25_B1024_R1' --hdim=256 --mode=1 --kn=25
# python3 include/bliss/construct.py --index='gist_epc40_K60_B1024_R1' --hdim=256 --mode=2 --kn=60

# python3 include/bliss/construct.py --index='crawl_epc40_K10_B2048_R1' --hdim=256 --mode=1 --kn=10
# python3 include/bliss/construct.py --index='crawl_epc40_K15_B2048_R1' --hdim=256 --mode=2 --kn=15

make query
taskset -c 0 ./query /scratch/gg29/data/sift/base.fvecs \
        /scratch/gg29/data/sift/label_base_3.txt \
        /scratch/gg29/data/sift/query.fvecs \
        /scratch/gg29/data/sift/label_query_3.txt \
        indices/sift1024kmeansMode1 \
        /scratch/gg29/data/sift/label_3_hard_groundtruth.ivecs \
        1024 kmeans 1 500

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 40000 80000 100000; do taskset -c 0 \
# ./query /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         /scratch/gg29/data/glove-100/query.fvecs \
#         /scratch/gg29/data/glove-100/label_query_3_0.1.txt \
#         indices/glove-100-1024kmeansMode1 \
#         /scratch/gg29/data/glove-100/label_3_0.1_hard_groundtruth.ivecs \
#         1024 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_glove-100VarAttr0.1.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 40000 80000 100000 120000; do taskset -c 0 \
# ./query /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         /scratch/gg29/data/glove-100/query.fvecs \
#         /scratch/gg29/data/glove-100/label_query_3_0.3.txt \
#         indices/glove-100-1024kmeansMode1 \
#         /scratch/gg29/data/glove-100/label_3_0.3_hard_groundtruth.ivecs \
#         1024 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_glove-100VarAttr0.3.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 40000 80000 100000 150000; do taskset -c 0 \
# ./query /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         /scratch/gg29/data/glove-100/query.fvecs \
#         /scratch/gg29/data/glove-100/label_query_3_0.5.txt \
#         indices/glove-100-1024kmeansMode1 \
#         /scratch/gg29/data/glove-100/label_3_0.5_hard_groundtruth.ivecs \
#         1024 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_glove-100VarAttr0.5.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 40000 80000 100000 180000; do taskset -c 0 \
# ./query /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         /scratch/gg29/data/glove-100/query.fvecs \
#         /scratch/gg29/data/glove-100/label_query_3_0.7.txt \
#         indices/glove-100-1024kmeansMode1 \
#         /scratch/gg29/data/glove-100/label_3_0.7_hard_groundtruth.ivecs \
#         1024 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_glove-100VarAttr0.7.txt

# for i in 500 700 1000 1500 2000 3000 4000 5000 8000 10000 20000 40000 80000 100000 200000; do taskset -c 0 \
# ./query /scratch/gg29/data/glove-100/base.fvecs \
#         /scratch/gg29/data/glove-100/label_base_3.txt \
#         /scratch/gg29/data/glove-100/query.fvecs \
#         /scratch/gg29/data/glove-100/label_query_3_0.9.txt \
#         indices/glove-100-1024kmeansMode1 \
#         /scratch/gg29/data/glove-100/label_3_0.9_hard_groundtruth.ivecs \
#         1024 kmeans 1 "$i"| sed -n 7p; done > results2/CAPSkmeans_glove-100VarAttr0.9.txt
