# Patricia Tree

[TOC]

###从 Trie 到 Patricia

在计算机科学中，Trie结构指的是基于关键空间分解的一种数据结构，中文翻译为字典树或者是前缀树，和大多数树形结构不一样的是，它使用边来表示信息，结点表示这个状态（字符串）存在与否或者是其余的信息。

基于这种存储结构，很显然一个节点的所有子孙都有相同的前缀，也就是这个节点对应的字符串，而根节点对应空字符串。

对于一个如下的键值对情形，我们可以生成这样的Trie树。
<div><center>![pic 1](http://opmza2br0.bkt.clouddn.com/17-5-29/33966793.jpg)</center></div>

这是一种查找效率非常高的数据结构，对于字符串的查找其最坏情况在 $O(w)$，其中 $w$ 是字典树中待查字符串的长度。

通常在实现 Trie 树的时候，我们通过保存字符集大小的`child`数组指针来逐个字符的实现 Trie 树的结点存储，但是这么做的会造成很大程度的空间消耗（这是一种以空间换时间的策略），因此智慧的后人便提出了许多的改进方式。

D. MORRSON 教授在 1968 年对 Trie 结构进行了改进，发明了 Trie 结构的一个变体  PATRICIA-TRIES (Practical Algorithm to Retrieve Information Coded in Alphanumeric)

Trie 树有许多只包含一个孩子的结点，为了提高空间的利用效率，Patricia 这种结构将一连串的『独生子女』压缩成一个结点，PATRICIA 树兄弟结点的关键字一定有相同的前缀，父结点的关键字是两个子结点关键字的公共前缀，所有叶结点的关键字长度相同而且是最大关键字长度。

其图示如下
<div><center>![pic 2](http://opmza2br0.bkt.clouddn.com/17-5-29/95055896.jpg)</center></div>


可以看到的是其中对于相同的前缀进行了压缩处理，只是从粗略的角度看，这种存储结构可以在实现与 Trie 树相同的时间效率下，达到更优的空间存储效率。

###自顶向下了解 Patricia 

通俗的讲，Patricia Tree 的逻辑特点如下。

1. 结点并不代表键（Key），由结点所延伸出的边代表键 Key 。
2. 结点可以包含键值对中的值（Value），且由一个终点标志位决定是否为包含信息的结点。
3. 如果一个结点只有一个子结点，那么它将会和这个子结点一起压缩成为一个结点，且这个过程逐级向下递推。
4. 兄弟结点之间一定有相同的关键字。

基于以上的特点，该数据结构能够高效的支持如下的操作。

#####1. 插入（Insertion）：
    
向PAT中插入一个字符串。

时间复杂度： $O(w)$ ，w为字符串长度。

#####2. 查找（Find）
    
在PAT中检索一个字符串

时间复杂度：$O(w)$，w为字符串长度。

#####3. 删除（Delete）

在PAT中删除一个指定字符串

时间复杂度：$O(w)$，w为字符串长度。

#####4. 修改（Update）
    
通过检索字符串来修改相应的Value

时间复杂度：$O(w)$，w为字符串长度。

#####5. **前缀查询（Prefix search）**
    
通过关键字前缀查询PAT里所有相关前缀单词。

时间复杂度：$O(w)$，w为含有这个前缀的最长字符串的长度。
<div><center>![pic 3](http://opmza2br0.bkt.clouddn.com/17-5-30/89445559.jpg)</center></div>

对于以上算法，其在字符串操作方面的时间效率都是及其高效的，是目前为止检索字符串最高效的一类数据结构，而 Patricia 相对于 Trie 在空间效率的改进上同样也是显而易见的。

###如何实现一个好用的 Patricia Tree

#####**一、存储结构定义**


在此次项目中，我们亲手实现了一个和网上相关代码都不太一样的 PAT Tree 并且测试的效果要高于 Github 上部分用户实现的源代码。

相关部分的定义如下。

```cpp
    template<class K,class V>
    struct patricia_node
    {
        K key;
        V value;
        std::map<char,patricia_node<K,V> *> childrens;
        bool terminal;
        patricia_node(K _key = K(), V _value = V(), bool _terminal = 1)
        {
            key = _key;
            value = _value;
            terminal = _terminal;
            childrens.clear();
        }
    };
```

在存储结构（即结点）定义上，我们定义了采取了传统的数据库查询方式，定义了键值对`(Key,Value)`。其中`key`用作结构信息的存储，`value`在终结结点被记录，可以通过查询键的方式获得值，`terminal` 作为终结结点的标志。

值得注意的是，也是本次我们实现的一个创新点，我们使用`std::map<char,patricia_node<K,V> *>`存储出现过的孩子结点的首字母，即`Key[0]`，这样做的好处在于我们可以避免对 alphabet 中的每一个字符都提前预置数组指针的空间，可以借用 STL 中现有的结构实现动态管理子结点的方式。

同样，`std::map<char,patricia_node<K,V> *>`帮助我们在查询的时候也快人一步，朴素方式实现的 PAT 中我们需要对根节点遍历一遍字符集合大小的空间以确定有没有这个字符`Key[0]`对应的子结点，这样在第一步时时间消耗为 $O( n )$的，n为字符集合大小，在使用`std::map<char,patricia_node<K,V> *>`之后我们就可以通过

```cpp
typename std::map<char,patricia_node<K,V> *> ::iterator itr = current_node->childrens.find(key[0]);
```
的操作来确认`childrens`中是否含有`key[0]`这个字符，这一步操作是 $O(\log_2{n})$ 的，n为字符集合大小。

通过这样的一种巧妙的方式，我们成功的又将 PAT 结构的各种操作的时间效率提升了常数时间，从 $n * w$ 提升到了 $\log_2{n} * w$，其中 n 为字符集合大小。

##### **二、具体操作实现**

下面我们接着介绍如何针对 PAT 的结构特点，来实现以上所提到的几个操作。

######1. 插入（Insertion）

在我们插入的时候，其从策略上遵从，寻找到第一个和当前已有结点有分支的结点，然后对当前结点进行分裂，但是在实现的过程中我们需要考虑以下的几种情况。

- 若树为空，直接创建一个叶子节点存储待插入字符串。
- 若插入的字符串首字符未出现在当前结点，创建一个新叶子节点存储这个字符串。

如下图，我们将`water`插入当前的PAT。
<div><center>![pic 4](https://upload.wikimedia.org/wikipedia/commons/3/30/Inserting_the_string_%27water%27_into_a_Patricia_trie.png)</center></div>

- 若插入的字符串 B 和当前已有字符串 A 可以前缀匹配，我们又需要细分几种情况进行讨论。
    1. B 是 A 的前缀，对于如下情形，test 是 tester 的前缀，我们需要在插入 test 的过程中对 tester 进行分裂，并且在 test 这个标签结束的位置设置一个终结标记。
    <div><center>![pic 5](https://upload.wikimedia.org/wikipedia/commons/5/5e/Insert_%27test%27_into_a_Patricia_trie_when_%27tester%27_exists.png)</center></div>
    2. A 是 B 的前缀，如下情形，我们插入 slower 这个字符串，此时这个 PAT 中已经存在着 slow 这个串，我们沿着 slow 遍历到终结的地方，给 slow 做一个终结标记，记录下 slow 对应的 value 值，接下来再向外延伸出 er 结点作为分支，并标记上结束标记以及对应的 value 值。
    <div><center>![pic 6](https://upload.wikimedia.org/wikipedia/commons/8/87/Insert_%27slower%27_with_a_null_node_into_a_Patricia_trie.png)</center></div>
    3. A 和 B 有部分公共的前缀，如下情形，我们试着插入 team ，A 串 test 和 B 串 team 有部分前缀重合 te，因此我们需要比对前缀重合的部分，将原结点 key 值修改为 te 然后切分出子串 am 和 st ，生成子结点 am 和 st，成为原结点的儿子，且将原结点的终结标记修改为 false。
    <div><center>![pic 7](https://upload.wikimedia.org/wikipedia/commons/0/01/Inserting_the_word_%27team%27_into_a_Patricia_trie_with_a_split.png)</center></div>

    **pesudo code** 

    ![pic 8](http://opmza2br0.bkt.clouddn.com/17-5-31/74305444.jpg)
    ![pic 9](http://opmza2br0.bkt.clouddn.com/17-5-31/23100128.jpg)

    上面算法中函数 LCP 为寻找两个字符串的最长公共前缀，例如字符串bool和boy的最长公共前缀为bo。

######2. 查找（Find）
    
查找的过程通过不断的匹配子串来完成，我们不断的对当前结点递归的进行子串匹配的操作，每次都尝试进行尽可能多的子串匹配，中间过程如果出现不匹配则返回false，否则这个过程就一直执行下去直到匹配到串的终点且终点结点的 terminal 标记为 true。
<div><center>![pic 8](https://upload.wikimedia.org/wikipedia/commons/6/63/An_example_of_how_to_find_a_string_in_a_Patricia_trie.png)</center></div>

**pesudo code**
![pic 10](http://opmza2br0.bkt.clouddn.com/17-5-31/63389931.jpg)

######3. 删除（Delete）
    
删除的实现首先需要我们去定位我们需要删除的结点，进行一次与 insert 相同的子串的匹配，但是实际操作的情形比 insert 时简单许多，因为如果子串不能和当前结点完全匹配，我们只需要输出 false。

当子串和当前结点长度相同且完全匹配时，我们删除这个结点，并根据其子结点的关系合并子结点到父节点上。

当子串比当前结点的长度更长时，截取出长的那一部分，然后我们递归的使用长出的这一部分进行新一次的删除操作。

这部分的操作和插入有些类似，我们留作思考。
（答案部分可以参考文末附上的代码链接）

######4. 前缀查询（Prefix search）
    
前缀查询也是 PAT 所支持的一种特性操作，其在实际生活中有十分广泛的应用。
![pic 11](http://opmza2br0.bkt.clouddn.com/17-5-31/94644861.jpg)

现在我们来分析如何实现这个操作，首先我们函数设计的接口应该是如下情形。


```cpp
std::vector<std::string> prefix_search(const std::string &prefix)
{
    std::vector<std::string> ret;
    prefix_search(prefix,root,ret);
    return ret;
}
```


首先我们的目标是根据prefix找到一个包含这个prefix的结点，

首先我们要沿着prefix，逐个从根节点向下进行匹配，如果prefix已经匹配的长度大于当前结点，那么我们继续向下拓展，直到拓展到第一个key的长度大于当前还未匹配的prefix长度的结点，**注意这里**，我们需要将已经走过的结点的前缀记录下来，存放到一个`ans_prefix`变量里面，然后对这个结点进行一次dfs。

在 dfs 的过程中记录 ans 串，当遇到终结标志时，不要停止 dfs 而是应该将这个 ans 串加入 ret 数组里面，因为对于树的 dfs 是无环的，所以我们可以在不用任何特判的 return 语句的情况下完成这次的 dfs。

注意，在dfs的过程中，我们记录的 ans 实际上是没有考虑 `ans_prefix`的，之后需要做一个处理，将记录下来的串都加上`ans_prefix`。
<div><center>![pic 12](https://upload.wikimedia.org/wikipedia/commons/6/63/An_example_of_how_to_find_a_string_in_a_Patricia_trie.png)</center></div>

### PAT VS Trie 实战测试
> 你说了那么多，我怎么知道 PAT 一定比 Trie 好呢？

本次实验中，我们针对插入，删除，查找三个基本操作，进行了时间效率的测试，最后还统计出了两种数据结构的空间消耗作为对比测试。

测试文本为 The snow of kilimanjaro by Hemingway.

全文大约有1.7W个单词。

##### 插入（Insertion）

首先是插入的对比测试，可以从图中看出的是，Patricia由于存在着比Trie更多的分裂结点的操作， 因此在插入时并不占优，此外由于我们实现的Patricia采取的动态分配内存的方式，而Trie采取的是默认静态空间存储，因此在动态分配内存的过程中也会相对更耗时一点，但也只是常数时间的细微差距。
![pic](/Users/luodian/Desktop/DSA/Patricia/Patricia/Insert.png)

##### 查找与删除（Find and delete）

接下来进行的是查找和删除的测试，在查找过程中，由于相同数据状态下 Patricia 的结构较 Trie 树结点数目少很多，因此查找时明显 Patricia 要更占优势。

在删除的时候，我们首先执行的是上一步的查找操作，因为 Patricia 在查找待删除串的过程会比 Trie 快出不少，而同时又因为 Trie 的结点数目更多，我们删除的过程中调用delete时的次数比 Patricia 要多出许多，因此 Patricia 在删除的过程中也更占优势。
![pic](/Users/luodian/Desktop/DSA/Patricia/Patricia/Find_delete.png)

##### 空间消耗（Space consume）

最后一个图进行的是空间消耗的比较，Trie 结构使用了接近250M的内存，而 Patricia 结构只使用了50M多一些，其中是接近于5倍的空间消耗的差距。
![pic](/Users/luodian/Desktop/DSA/Patricia/Patricia/Space.png)




### 所以，Patricia有哪些实际的应用呢？

##### 信息检索（Infomation retrieval）

对于一篇文章，我们可以将它进行分词（针对于中文，英文不用考虑分词），然后将文章转换为单词存入PAT结构中,value存放单词出现的位置，对于重复出现的value，我们可以考虑将`(K,V)`中的 V 的类型设置为 `std::vector<int>` ，来逐个存放单词出现的位置，这样做之后我们可以快速的检索文中单词出现的次数，位置等信息。

而且还支持动态的增删改查。

> 比KMP快到不知道哪里去了。

##### 前缀查询（Prefix search）
    
前缀查询广泛的应用于词典软件，搜索引擎当中，这部分的应用与我们的日常生活息息相关。
<div><center>![pic 14](http://opmza2br0.bkt.clouddn.com/17-5-31/69276475.jpg)</center></div>


##### [梅克尔树（Merkle Tree）](https://en.wikipedia.org/wiki/Merkle_tree)

Merkle Tree可以看做Hash List的泛化（Hash List可以看作一种特殊的Merkle Tree，即树高为2的多叉Merkle Tree）。

在最底层，和哈希列表一样，我们把数据分成小的数据块，有相应地哈希和它对应。但是往上走，并不是直接去运算根哈希，而是把相邻的两个哈希合并成一个字符串，然后运算这个字符串的哈希，这样每两个哈希就结婚生子，得到了一个”子哈希“。如果最底层的哈希总数是单数，那到最后必然出现一个单身哈希，这种情况就直接对它进行哈希运算，所以也能得到它的子哈希。于是往上推，依然是一样的方式，可以得到数目更少的新一级哈希，最终必然形成一棵倒挂的树，到了树根的这个位置，这一代就剩下一个根哈希了，我们把它叫做 Merkle Root。

在p2p网络下载网络之前，先从可信的源获得文件的Merkle Tree树根。一旦获得了树根，就可以从其他从不可信的源获取Merkle tree。通过可信的树根来检查接受到的Merkle Tree。如果Merkle Tree是损坏的或者虚假的，就从其他源获得另一个Merkle Tree，直到获得一个与可信树根匹配的Merkle Tree。

值得一提的是，以太坊提出的比特币验证方案正是用了五颗 Merkle Tree 来分别管理交易，状态，账单等等信息。

更多请了解，[谈谈以太坊的Merkle树](http://www.8btc.com/merkling-in-ethereum)
<div><center>![pic 13](http://images2015.cnblogs.com/blog/834896/201605/834896-20160527163936819-725283544.png)</center></div>

##### 路由寻路（Best-Matching Prefix (BMP)）
    
在网络方面有一个十分棘手的问题叫做IP Routing Lookup Algorithms，就是路由器需要对接收到的包，需要快速的对其IP地址在路由表中查询最长匹配的前缀，以确定一个准确的终点地址。

当同时考虑到并发性和效率的时候，以前采用过的许多方法都显得不那么高效，因为路由可能会在一秒钟之内接到上千个包，而一半路由表中存放的元素大约有30000多项。

这实际上就是一个不那么纯粹的前缀匹配的问题，只是最终结点的匹配是模糊的，我们可以在 $O(w)$ 的时间内同时匹配出多个可能的结果，通过计算失配的字符数目我们可以得到一个最小失配串，作为我们 BMP 查询的结果。

    Consider the routing table has entries for the prefixes,
    P1 = 0101
    P2 = 0101101
    P3 = 010110101011
    The BMP (Best Matching Prefix) for 010101101011 is P1
    The BMP (Best Matching Prefix) for 010110101101 is P2

##### 子网查询（IP subnet lookup）

对于一个给定的输入串，比如为 192.168.0.0，我们可以通过使用 PAT 树去查询所有 IP 所属192.168.....下的子网IP，这就是基于之前前缀查询的应用。

值得一提的是，在网络上我们找到了一道百度公司出的相关面试题。

题目大意是维护100亿个url地址，比如http://www.baidu.com/s?wd=baidu的属性，包括定长属性（比如其被系统发现的时间）和不定长属性（比如其描述）实现一个系统来做到

 - 储存和维护100亿个 url 及其属性。 
 - 实现 url 及其属性的增删改。 
 - 查一个 url 是否在系统中并给出信息。 
 - 快速选出一个站点下所有 url。

首先，这些 url 要经过 partition 分到 X 台机器中：考虑使用一个 hash 函数 hash (hostname(url)) 将 url 分配到 X 台机器中，这样做的目的：一是数据的分布式存储，二是同一个站点的所有 url 保存到同一台机器中。

其次，每台机器应该如何组织这些数据？一种思路是用数据库的思路去解决，这里提供另外一种思路。考虑将 url 直接放在内存，接将 url 组织成树状结构，显然为了满足快速选出一个站点下所有 url 这个性质，我们就必须得用 Trie 结构了，但是考虑到数据规模是 100 亿，因此我们不得不选择 PAT 结构，相比如传统 Trie 可以节约大量空间。
    

### 还有比它更厉害的改进吗？
> 答案是，当然有，人类的智慧是无穷无尽的，神一样的前辈们和潮水一样的后继者，请接受我的膝盖。

- HAT-Trie 是一种利用缓存来提升存取速度的数据结构，
- Adaptive radix tree 是一种动态管理儿子结点的Trie结构，它可以动态的根据子结点的个数分配内存，每个结点存儿子结点的指针都是动态的，这样做可以更好的节省内存，这种结构既可以用在 Trie 上也可以用在 Patricia 上，在本次的 Patricia tree 中我们实现的就是一个 Adaptive radix tree 结构，我们使用了`std::map<char,patricia_tree<K,V> *>`来管理儿子节点，并且提供 $O(\log_{n})$ 的索引速度。 

### Qt应用展示

有请武德浩，浩哥登场为我们展示我们结合 Qt 做出的应用。

啪啪啪啪（是👏，不要想太歪）

### 写在最后
####一、团队协作的一些感受
#####1. 团队分工明确，合作过程痛并愉快着

根据计算机理论中异步的思想，我们一开始制定了一个比较科学的分工，本次实验我（李博）在前期主要负责 Patricia Tree 这个类以及作为比较的 Trie Tree 类的结构设计与代码实现，前期给武德浩留出了准备 Qt 和设计应用的时间，当我完成了数据结构相关的代码准备工作，并将设计接口交接给武德浩之后，我就直接开始了准备文档，报告，PPT 的工作，此时我和武德浩并行的处理着我们的项目，他一边攻克着 Qt 当中所遇到的无数难题，我一边浏览着无数相关 Patricia Tree 的各路博客与Paper，准备我们最后的文档部分，也就是这篇万字（xia che）长文。

虽然我们都很累，但是！胜在队友给力，负责，敢于面对困难，迎难而上，中间过程中并未出现进程死锁，单方面闲置的情况。

我们才有了把这个项目做到尽善尽美的决心与毅力。

#####2. 所以到底遇到了哪些困难呢？

武德浩：

李博：

1. 这次代码部分主要遇见的困难在于网上相关资料极少，我们能够找到的源代码大部分近乎不可读，讲解十分的少，导致我们在前期的时候发育极其缓慢，再加上我决定使用动态管理儿子结点的方法，这是在网上都从来没见过的一种做法，包括像前缀查询部分也是如此，甚至连伪代码都找不到，讲解极少，没有中文分析的博客，这些都是我第一次遇见的把，毕竟这个数据结构真的很生僻。因此我很多时候只能凭着自己的推理，结合着网上能够找到的一些代码，摸着石头过河来完成这个数据结构，但还好在小数据范围的测试中还没有遇到 bug，这是让我非常开心的。

2. 在写文档的时候突然发现这个数据结构应用原来这么多，以太坊，router之类的都不太熟悉，于是又被迫展开了一场广泛的学习，而同样遇到的困难就是，这部分应用仍然极少有中文资料，百度能够搜索到的都是抄来抄去，干货极少，搜索英文关键词才能够找到想要的结果，果然又一次的强调了英文对于计算机学生的重要性！

####二、感谢帮助过我们的博客及Paper

- [PATRICIA—Practical Algorithm To Retrieve Information Coded in Alphanumeric, DONALD g. MORRISON, Sandia Laboratory](http://dl.acm.org/citation.cfm?id=321481)
- [Crit-bit trees](http://cr.yp.to/critbit.html)
- [Prefix search with patricia trie](http://groonga.org/docs/tutorial/patricia_trie.html)
- [Trie from wiki](https://en.wikipedia.org/wiki/Trie)
- [Radix tree from wiki](https://en.wikipedia.org/wiki/Radix_tree)
- [Performance Analysis of IP Routing Lookup Algorithms : Patricia Tree based vs. Hashing based, Packer Ali, Dhamim
Department of Mathematics & Computer Science, Kent State University April 2000](http://webdiis.unizar.es/asignaturas/TAP/material/ip_report.pdf)
- [Patricia Trie perl module for fast IP address lookups](http://search.cpan.org/~plonka/Net-Patricia-1.014/Patricia.pm)
- [谈谈以太坊的Merkle树](http://www.8btc.com/merkling-in-ethereum)
- [Merkle tree from wiki](https://en.wikipedia.org/wiki/Merkle_tree)
- [FreeBSD Kernel](https://www.freebsd.org/cgi/man.cgi?query=rtfree&apropos=0&sektion=9&manpath=FreeBSD%2011-current&format=html)

**最后还要感谢一直帮助我们的助教林可学长和一年来一直在关心激励我们的张岩老师**
















