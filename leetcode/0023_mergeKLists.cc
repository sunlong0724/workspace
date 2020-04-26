#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>

#include <vector>
using namespace std;

struct ListNode{
    int val;
    ListNode* next;
    ListNode(int val):next(nullptr){
        this->val = val;
    }
    ListNode(int val, ListNode* next){
        this->val = val;
        this->next = next;
    }
};

ListNode* merge2Lists(ListNode* l1, ListNode* l2){
    ListNode dummy(0);
    ListNode* p = &dummy;
    while(l1 && l2){
        if (l1->val < l2->val){
            p->next = l1;
            l1 = l1->next;
        }else{
            p->next = l2;
            l2 = l2->next;
        }

        p = p->next;
    }
    p->next = l1?l1:l2;
    return dummy.next;
}



ListNode* mergeKLists(vector<ListNode*>& lists){
   // fprintf(stdout,"%s\n", __FUNCTION__);
    int len = lists.size();
    //fprintf(stdout,"%d, ", len);
    if (len == 0) return nullptr;
    if (len == 1) return *lists.begin();
    int j = 0;
    for (int i = 0 ; i < len-1; i+=2, ++j){
        auto l1 = lists[i];
        auto l2 = lists[i+1];
        lists[j] = merge2Lists(l1, l2);
    }
    if (len & 0x01) lists[j++] = lists[len-1];
    lists.erase( lists.begin() + j, lists.end() );
    return mergeKLists(lists);

}

int main(int argc, char** argv)
{
    ListNode arr[] = {{1,&arr[1]}, {2, &arr[2]}, {3,&arr[3]}, {4, &arr[4]}, {5,nullptr}};
    ListNode arr2[] = {{1,&arr2[1]}, {2, &arr2[2]}, {3,&arr2[3]}, {4, &arr2[4]}, {5,nullptr}};
    vector<ListNode*>  lists = {arr, arr2};
    ListNode* new_head = mergeKLists(lists);
    ListNode* p = new_head;
    while(p){
        fprintf(stdout, "%d, ", p->val);
        p = p->next;
    }
    
    printf("main done!\n");
    return 0;
}

