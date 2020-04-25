/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
public:

    /*双指针A B同时向前，A到尾时指向b的开始，B到尾时指向a的开始，这样就可以便宜出a b中较长链表多出的部分。
    A B同时指向的同一个节点即为相交节点。*/
   ListNode *getIntersectionNode(ListNode *headA, ListNode *headB) {
        ListNode *pa,*pb;
        pa = headA;
        pb = headB;
        while(pa != pb){
            if (pa){
                pa = pa->next;
            }else{
                pa = headB;
            }
            if (pb){
                pb = pb ->next;
            }else{
                pb = headA;
            }
        }

        return pa;
    }
  
    /*
    1. 最直观想法：将较长链表的多出部分偏移出去，然后双指针同时向后遍历，第一个地址相等的节点即为所求。
    
  */
    ListNode *getIntersectionNode(ListNode *headA, ListNode *headB) {
        int len_a = 0,len_b = 0;
        ListNode* pa = headA;
        while(pa){
            len_a++;
            pa = pa->next;
        }
        ListNode* pb = headB;
        while(pb){
            len_b++;
            pb = pb->next;
        }
        int diff = len_a-len_b;
        pa = headA;
        pb = headB;
        if (diff > 0){
            while(diff > 0){
                pa = pa->next;
                diff--;
            }
        }else{
            while(diff < 0){
                pb = pb->next;
                diff++;
            }
        }
        while(pa && pb && pa != pb){
            pa = pa->next;
            pb = pb->next;
        }
        return pa;
    }
};
