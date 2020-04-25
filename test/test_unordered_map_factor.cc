#include <unordered_map>

using namespace std;
int main(int argc, char** argv){
    unordered_map<int,int> hash_map;
    hash_map.max_load_factor(20);//容器内元素总个数与bucket的比值。

    
    for (int i = 0; i < 300; ++i){
        hash_map.insert({i,i});
        fprintf(stdout,"插入:%d, 桶索引:%u,桶数量:%u,factor:%f\n",i, hash_map.bucket_count()
                ,hash_map.max_bucket_count(),hash_map.load_factor());

    }
}
