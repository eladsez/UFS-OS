#include <string.h>
#include "myfs.h"



int main(){
    mymkfs(10*1024);
    mymount("UFS", "", "", 0, NULL);
}
