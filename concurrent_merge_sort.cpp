#include<bits/stdc++.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>
using namespace std;


void selection_sort(int* arr,int n){
    for(int i=0;i<n-1;i++){
        int min_idx=i;
        for(int j=i+1;j<n;j++){
            if(arr[min_idx]>arr[j])
                min_idx=j;
        }
        int temp=arr[i];
        arr[i]=arr[min_idx];
        arr[min_idx]=temp;
    }
}

void merge(int array[], int l, int m, int h){
    int count=h-l+1;
    int temp[count];
    int i=l, k=m+1;
    int idx=0;
    while (i<=m && k<=h){
        if (array[i]<array[k])
            temp[idx++]=array[i++];
        else if (array[k]<array[i])
            temp[idx++]=array[k++];
        else if (array[i]==array[k]){
            temp[idx++]=array[i++];
            temp[idx++]=array[k++];
        }
    }
  
    while (i<=m)
        temp[idx++]=array[i++];
  
    while (k<=h)
        temp[idx++]=array[k++];
  
    for (i=0; i<count; i++)
        array[l++] = temp[i];
}

void mergesort(int* array,int start,int end){
    int len=end-start+1;
    int mid=start+len/2-1;
    if(len<=5){
        selection_sort(array+start,len);
        return;
    }
    pid_t lpid,rpid;
    lpid=fork();
    if(lpid<0){
        cout<<"Error in creating left child of array from "<<start<<" to "<<end<<endl;
        exit(1);
    }
    else if(lpid==0){
        mergesort(array,start,mid);
        exit(0);
    }
    else{
        rpid=fork();
        if(rpid<0){
            cout<<"Error in creating right child of array from "<<start<<" to "<<end<<endl;
            exit(1);
        }
        else if(rpid==0){
            mergesort(array,mid+1,end);
            exit(0);
        }
    }

    int* status;
    waitpid(lpid,status,0);
    waitpid(rpid,status,0);
    merge(array,start,mid,end);

}


int main(){

    int shmid;
    key_t key=IPC_PRIVATE;
    int* shm_array;
    int n;
    cout<<"Enter number of elements in test array"<<endl;
    cin>>n;
    
    size_t shm_size=sizeof(int)*n;
    if((shmid=shmget(key,shm_size,IPC_CREAT|0666))<0){
        cout<<"Error in shmget"<<endl;
        exit(1);
    }
    if((shm_array=(int*)shmat(shmid,(void*)0,0))==(int*) -1){
        cout<<"Error in shmat"<<endl;
        exit(1);
    }

    srand(time(0));
    for(int i=0;i<n;i++){
        shm_array[i]=rand();
    }
    mergesort(shm_array,0,n-1);
    for(int i=0;i<n;i++){
        cout<<shm_array[i]<<" ";
    }
    cout<<endl;

    if (shmdt(shm_array) == -1){
        cout<<"error in shmdt"<<endl;
        exit(1);
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1){
        cout<<"Error in shmctl"<<endl;
        exit(1);
    }

    return 0;
}