/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-10-02 00:31:43
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-03 00:26:27
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\CommonFunc\SystemUtil\bget.h
 *ModifyRecord1:    
******************************************************************/
/*

    Interface definitions for bget.c, the memory management package.

*/

#ifndef _
#ifdef PROTOTYPES
#define  _(x)  x		      /* If compiler knows prototypes */
#else
#define  _(x)  ()                     /* It it doesn't */
#endif /* PROTOTYPES */
#endif

typedef long bufsize;

/*使用从buffer开始的存储， 创建一个len字节的缓冲池。您可以随后调用以为整个缓冲池贡献额外的存储空间*/
void	bpool	    _((void *buffer, bufsize len));
/*分配大小为字节的缓冲区。返回缓冲区的地址，或者NULL,如果没有足够的内存来分配缓冲区*/
void   *bget	    _((bufsize size));
/*分配大小为字节的缓冲区，并将其清除为全零。返回缓冲区的地址，或者NULL,如果没有足够的内存来分配缓冲区*/
void   *bgetz	    _((bufsize size));
/*重新分配先前由分配的缓冲区bget()，将其大小更改为newsize并保留所有现有数据。 如果没有足够的内存可用于重新分配缓冲区，则返回NULL，在这种情况下，原始缓冲区保持不变*/
void   *bgetr	    _((void *buffer, bufsize newsize));
/*将先前由分配 的缓冲区buf返回bget()到可用空间池*/
void	brel	    _((void *buf));
/*该函数用于分配失败时，扩展堆容量。首先，当一次堆内存分配失败时，如果compact非空，则自动被调用，其参数sizereq是当前分配失败的内存（header+body），
**sequence是连续分配失败的次数。bectl允许compact去brel、bpool让这次分配得以成功。当compact返回非0时，则会重试bget；当compact返回0时，则分配请求失败，它将触发acquire被调用。
**acquire函数指针参数为所需堆内存大小，返回一个指针。它的内部逻辑是扩展堆池大小，如果acquire返回NULL，代表整个分配失败。
**pool_incr指定了堆扩展的块大小*/
void	bectl	    _((int (*compact)(bufsize sizereq, int sequence),void *(*acquire)(bufsize size),void (*release)(void *buf), bufsize pool_incr));
/*该函数用于获取bget的状态。curalloc指向当前已分配的堆大小；totfree指向当前空闲堆大小；maxfree指向空闲堆中最大的块大小；nget为调用bget成功的次数；nrel为调用brel成功的次数。*/
void	bstats	    _((bufsize *curalloc, bufsize *totfree, bufsize *maxfree,long *nget, long *nrel));
/*pool_incr指向扩展块大小；npool指向堆池有多少个块；npget和nprel分别为在请求和释放扩展块的次数；nprel为释放的次数；ndget和ndrel分为为在acquire和release中调用bget和brel的次数*/
void	bstatse     _((bufsize *pool_incr, long *npool, long *npget,long *nprel, long *ndget, long *ndrel));
/*通过指向缓冲器BUF被倾倒在标准输出*/
void	bufdump     _((void *buf));
/*先前由bpool（）调用初始化的缓冲池池中的所有缓冲区均以升序的内存地址顺序列出。
**如果dumpalloc为非零，则转储已分配缓冲区的内容；否则，转储为0。如果dumpfree为非零，则转储空闲块的内容。*/
void	bpoold	    _((void *pool, int dumpalloc, int dumpfree));
/*先前由调用初始化的命名缓冲池将 bpool()针对错误的指针，覆盖的数据等进行验证。如果NDEBUG未定义进行编译，则任何错误都会产生断言失败。
**否则，如果池有效，则返回1，如果发现错误，则返回0。*/
int	bpoolv	    _((void *pool));
