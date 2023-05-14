class config:
    DATASET = {'glove-100': {'N':1183514, 'd':100 , 'metric': 'L2', 'dt':'float32'},           
            'sift': {'N':1000000, 'd':128 , 'metric': 'L2', 'dt':'float32'},
            'gist': {'N':1000000, 'd':960 , 'metric': 'L2', 'dt':'float32'},  
            'crawl': {'N':1989995, 'd':300 , 'metric': 'L2', 'dt':'float32'}, 
            'audio': {'N':53387, 'd':192 , 'metric': 'L2', 'dt':'float32'},
            'msong': {'N':992272, 'd':420 , 'metric': 'L2', 'dt':'float32'}    
            }  

# UQ-V 256 1000000 10000 7.2 Video + Attributes
# Msong 420 992272 200 9.5 Audio + Attributes
# Audio 192 53387 200 5.6 Audio + Attributes
# SIFT1M 128 1000000 10000 9.3 Image + Attributes
# GIST1M 960 1000000 1000 18.9 Image + Attributes
# Crawl 300 1989995 10000 15.7 Text + Attributes
# GloVe 100 1183514 10000 20.0 Text + Attributes
# Enron 1,369 94987 200 11.7 Text + Attributes
# Paper 200 2029997 10000 - Text + Attributes