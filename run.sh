./a.out tuple_text_to_binary_csr/wiki_vote_beg_pos.bin tuple_text_to_binary_csr/wiki_vote_csr.bin 
./dfs_m tuple_text_to_binary_csr/edge_beg_pos.bin tuple_text_to_binary_csr/edge_csr.bin 

g++ -o dfs_baseline dfs_baseline.cpp -lpthread -std=c++11
g++ -o compare compare.cpp