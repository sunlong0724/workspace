
struct ListNode{
    int val;
    ListNode* next;
    ListNode(int val):next(nullptr){
        this->val = val;
    }
    ListNode(int val, void* next){
        this->val=val;
        this->next = (ListNode*)next;
    }
};

#include <stdio.h>
#include <algorithm>
using namespace std;

void do_quick_sort(ListNode* head, ListNode* tail){
    if (head == tail || head->next == tail) return ;
    int pivot = head->val;
    ListNode* cur = head, *p = head;
    while(p != tail){
        if (p->val < pivot){
            cur = cur->next;
            swap(cur->val, p->val);
        }
        p = p->next;
    }
    
    swap(head->val, cur->val);

    do_quick_sort(head, cur);
    do_quick_sort(cur->next, tail);
}

ListNode* merge_2_list(ListNode* l1, ListNode* l2){
    ListNode dummy(0);
    ListNode* p = &dummy;
    while(l1 && l2){
        if (l1->val < l2->val){
            p->next = l1;
            l1=l1->next;
        }else{
            p->next = l2;
            l2 = l2->next;
        }
        p = p->next;
    }

    p->next = (l1?l1:l2);
    return dummy.next;
}

ListNode* do_merge_sort(ListNode* head){
    if (head == nullptr || head->next == nullptr ) return head;
    ListNode* slow = head, *fast = head->next;
    while(fast && fast->next){//1, 2, 3, 4, 5
        slow= slow->next;
        fast = fast->next->next;
    }
    ListNode* right = do_merge_sort(slow->next);
    slow->next = nullptr;
    ListNode* left = do_merge_sort(head);

    return merge_2_list(left,right);
}


ListNode* sortList(ListNode* head){
    //考虑在O(nlogn)时间复杂度下，考虑快速排序和归并排序
    //do_quick_sort(head, nullptr);
    return do_merge_sort(head);
    
    return head;
}



int main(int arg,char** argv){
    ListNode nodes[] = {{4,&nodes[1]}, {2,&nodes[2]}, {1,&nodes[3]}, {3,nullptr}};

    ListNode* p = sortList(nodes);

    while(p){
        fprintf(stdout,"%d, ", p->val);
        p = p->next;
    }
    fprintf(stdout,"\n");

    return 0;

}

