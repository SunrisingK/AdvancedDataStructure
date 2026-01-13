#include "./skiplist.h"
using namespace skip_list;


int main(int argc, char const* argv[]) {
    // 创建跳表对象, 最大高度为10
    SkipList<int, std::string> skipList(10);

    skipList.loadFile();
    skipList.displaySkipList();

    skipList.insertNode(1, "是徒为静养");
    skipList.insertNode(3, "而不用克己工夫也");
    skipList.insertNode(7, "如此");
    skipList.insertNode(8, "临事便要倾倒");
    skipList.insertNode(9, "人需在事上磨");
    skipList.insertNode(11, "事以秘成");
    skipList.insertNode(12, "尽人事知天命");
    skipList.insertNode(15, "天涯去后");
    skipList.insertNode(16, "乡关外");
    skipList.insertNode(17, "听风声诉幽怀");
    skipList.insertNode(18, "繁华落尽终是一场空");
    skipList.insertNode(19, "方可立得住");

    skipList.dumpFile();

    std::cout << "skip_list size:" << skipList.size() << std::endl;

    skipList.searchNode(9);
    skipList.searchNode(18);
    skipList.searchNode(29);

    skipList.displaySkipList();

    skipList.deleteNode(7);
    skipList.deleteNode(19);
    std::cout << "skip_list size:" << skipList.size() << std::endl;
    
    skipList.displaySkipList();

    return 0;
}