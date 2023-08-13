#ifndef SHADEWATCHER_UTIL_LOCALSTORE_H_
#define SHADEWATCHER_UTIL_LOCALSTORE_H_

#include "parser/kg.h"
#include "file.h"

typedef std::unordered_map<hash_t, int64_t, HashFunction> nodemap;

class LocalStore
{
public:
    std::string kg_path;
    KG *infotbl;

    nodenum_t node_num;
    nodenum_t proc_num;
    nodenum_t socket_num;
    nodenum_t file_num;
    event_t edge_num;
    nodemap node_map;
    nodemap new_node_map;

    int numTrainLines = 0;

    LocalStore(std::string, KG *);

    void StoreProc();
    void StoreFile();
    void StoreSocket();
    void StoreEdge(int);
    void StoreEntityFile();
    void StoreInteractionFile();
    void StoreRelationFile();
    void StoreTrainFile();
    void StoreRecommendationModelFiles();
    void SetNodeMap();

    void AppendNodeMap(int);
    void AppendEntityFile();
    void StoreEntityFileChecker();
    // store KG to local files
    void KGStoreToFile(int = 0);

    // new
    void StoreMaliciousTruthFile();

    // store Darpa system entity information
    void EntityStoreToFile();

    // Count the number of procs, files, sockets, and edges
    void DumpProcFileSocketEdge2FactSize(int = 0);

    void StoreNodeHashToUuidFile();
};

// load KG from local files
void KGLoadFromFile(std::string, KG *);

// load Darpa system entity information
void EntityLoadFromFile(std::string, KG *);

#endif
