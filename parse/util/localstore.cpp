#include "localstore.h"

LocalStore::LocalStore(std::string _kg_path, KG *_infotbl)
{
	kg_path = _kg_path;
	infotbl = _infotbl;

	node_num = 0;
	proc_num = 0;
	socket_num = 0;
	file_num = 0;
	edge_num = 0;
}

// Encode nodes in ProcNodeTable by one-hot-encoding and write the results into procfact_tmp.txt
void LocalStore::StoreProc()
{
	std::cout << "\tstoring process entity" << std::endl;
	std::string procfact_path = kg_path + "/procfact_tmp.txt";
	std::ofstream procfact_file(procfact_path, std::ios::app);
	if (!procfact_file.is_open())
	{
		std::cerr << "Fail to open file:" << procfact_path << std::endl;
		return;
	}

	std::string nodefact_path = kg_path + "/nodefact_tmp.txt";
	std::ofstream nodefact_file(nodefact_path, std::ios::app);
	if (!nodefact_file.is_open())
	{
		std::cerr << "Fail to open file:" << nodefact_path << std::endl;
		return;
	}

	for (auto proc : infotbl->ProcNodeTable)
	{
		hash_t id = proc.first;
		std::string pid = proc.second->pid;
		std::string exe = proc.second->exe;
		std::string ppid = proc.second->ppid;
		std::string args = proc.second->args;
		procfact_file << id << " " << pid << " " << exe
					  << " " << ppid << " " << args << std::endl;
		nodefact_file << id << " " << 1 << std::endl;
	}
	procfact_file.close();
	nodefact_file.close();
}

// Encode nodes in FileNodeTable by one-hot-encoding and write results into filefact_tmp.txt
void LocalStore::StoreFile()
{
	std::cout << "\tstoring file entity" << std::endl;
	std::string filefact_path = kg_path + "/filefact_tmp.txt";
	std::ofstream filefact_file(filefact_path, std::ios::app);
	if (!filefact_file.is_open())
	{
		std::cerr << "Fail to open file:" << filefact_path << std::endl;
		return;
	}

	std::string nodefact_path = kg_path + "/nodefact_tmp.txt";
	std::ofstream nodefact_file(nodefact_path, std::ios::app);
	if (!nodefact_file.is_open())
	{
		std::cerr << "Fail to open file:" << nodefact_path << std::endl;
		return;
	}

	// auto node_num = infotbl->FileNodeTable.size();
	// filefact_file << node_num << std::endl;

	for (auto file : infotbl->FileNodeTable)
	{
		hash_t id = file.first;
		std::string name = file.second->name;
		std::string version = file.second->version;
		filefact_file << id << " " << name << " " << version << std::endl;
		nodefact_file << id << " " << 2 << std::endl;
	}
	filefact_file.close();
	nodefact_file.close();
}

// Encode nodes in SocketNodeTable by one-hot-encoding and write results into socketfact_tmp.txt
void LocalStore::StoreSocket()
{
	std::cout << "\tstoring socket entity" << std::endl;
	std::string socketfact_path = kg_path + "/socketfact_tmp.txt";
	std::ofstream socketfact_file(socketfact_path, std::ios::app);
	if (!socketfact_file.is_open())
	{
		std::cerr << "Fail to open file:" << socketfact_path << std::endl;
		return;
	}

	std::string nodefact_path = kg_path + "/nodefact_tmp.txt";
	std::ofstream nodefact_file(nodefact_path, std::ios::app);
	if (!nodefact_file.is_open())
	{
		std::cerr << "Fail to open file:" << nodefact_path << std::endl;
		return;
	}

	// auto node_num = infotbl->SocketNodeTable.size();
	// socketfact_file << node_num << std::endl;

	for (auto socket : infotbl->SocketNodeTable)
	{
		hash_t id = socket.first;
		std::string name = socket.second->name;
		socketfact_file << id << " " << name << std::endl;
		nodefact_file << id << " " << 3 << std::endl;
	}
	socketfact_file.close();
	nodefact_file.close();
}

// Store edge facts in KGEdgeTable and write results into filefact_tmp.txt
void LocalStore::StoreEdge(int _file_id)
{
	std::cout << "\tstoring edges" << std::endl;
	std::string file_id = std::to_string(_file_id);
	std::string edgefact_path = kg_path + "/edgefact_tmp_" + file_id + ".txt";
	std::ofstream edgefact_file(edgefact_path, std::ios::app);
	if (!edgefact_file.is_open())
	{
		std::cerr << "Fail to open file:" << edgefact_path << std::endl;
		return;
	}

	// auto edge_num = infotbl->KGEdgeTable.size();
	// edgefact_file << edge_num << std::endl;

	for (const auto it : infotbl->KGEdgeTable)
	{
		KGEdge *edge = it.second;
		hash_t _e_id = edge->e_id;
		hash_t _n1_hash = edge->n1_hash;
		hash_t _n2_hash = edge->n2_hash;
		std::string e_id = std::to_string(_e_id);
		std::string n1_hash = std::to_string(_n1_hash);
		std::string n2_hash = std::to_string(_n2_hash);
		std::string relation = std::to_string(EdgeEnum2Int(edge->relation));
		std::string sequence = uint128tostring(edge->seq);
		std::string session = std::to_string(edge->sess);
		std::string ts = edge->timestamp;
		edgefact_file << e_id << " " << n1_hash << " " << n2_hash << " " << relation
					  << " " << sequence << " " << session << " " << ts << std::endl;
	}
	edgefact_file.close();
}

void LocalStore::KGStoreToFile(int file_id)
{
	auto start = OverheadStart();
	std::cout << "Store KG information in " + kg_path + "/" << std::endl;
	StoreProc();
	StoreFile();
	StoreSocket();
	StoreEdge(file_id);
	proc_num += infotbl->ProcNodeTable.size();
	file_num += infotbl->FileNodeTable.size();
	socket_num += infotbl->SocketNodeTable.size();
	node_num = proc_num + file_num + socket_num;
	edge_num += infotbl->KGEdgeTable.size();
	OverheadEnd(start, "Store KG to files");
}

// Darpa Dataset: Store system entity information into local files, so that we
// can run parser on arbitrary files (e.g., ta1-trace-e3-official.json.125)
void LocalStore::EntityStoreToFile()
{
	auto start = OverheadStart();
	std::cout << "Store system entity information in " + kg_path + "/" << std::endl;
	StoreProc();
	StoreFile();
	StoreSocket();

	proc_num += infotbl->ProcNodeTable.size();
	file_num += infotbl->FileNodeTable.size();
	socket_num += infotbl->SocketNodeTable.size();
	node_num = proc_num + file_num + socket_num;
	OverheadEnd(start, "Store system entity to files");
}

void KGLoadFromFile(std::string kg_path, KG *infotbl)
{
	auto start = OverheadStart();
	std::cout << "Load system entity and edges facts" << std::endl;
	std::string proc_path = kg_path + "/procfact.txt";
	std::string file_path = kg_path + "/filefact.txt";
	std::string socket_path = kg_path + "/socketfact.txt";
	std::string _edge_path = kg_path + "/edgefact_";

	// read facts
	std::string line;

	std::ifstream proc_file(proc_path, std::ios::in);
	if (!proc_file.is_open())
	{
		std::cerr << "Fail to open file:" << proc_path << std::endl;
		exit(0);
	}
	std::getline(proc_file, line);
	nodenum_t proc_num = std::stoi(line);

	std::ifstream file_file(file_path, std::ios::in);
	if (!file_file.is_open())
	{
		std::cerr << "Fail to open file:" << file_path << std::endl;
	}
	std::getline(file_file, line);
	nodenum_t file_num = std::stoi(line);

	std::ifstream socket_file(socket_path, std::ios::in);
	if (!socket_file.is_open())
	{
		std::cerr << "Fail to open file:" << socket_path << std::endl;
	}
	std::getline(socket_file, line);
	nodenum_t socket_num = std::stoi(line);

	// For Darpa, there should be a diff way to load
	std::cout << "\tLoad process entity" << std::endl;
	for (nodenum_t it = 0; it < proc_num; it++)
	{
		std::getline(proc_file, line);
		std::istringstream proc(line);
		std::string p_hash_str;
		proc >> p_hash_str;
		hash_t p_hash = std::stol(p_hash_str);

		std::string pid;
		proc >> pid;

		std::string exe;
		proc >> exe;

		std::string ppid;
		proc >> ppid;

		std::string args;
		proc >> args;

		NodeProc *p_temp = new NodeProc(pid, exe, args, ppid, p_hash);
		infotbl->InsertProc(p_temp);
	}

	std::cout << "\tLoad file entity" << std::endl;
	for (nodenum_t it = 0; it < file_num; it++)
	{
		std::getline(file_file, line);
		std::istringstream file(line);
		std::string f_hash_str;
		file >> f_hash_str;
		hash_t f_hash = std::stol(f_hash_str);

		std::string f_name;
		file >> f_name;

		std::string f_version;
		file >> f_version;

		NodeFile *f_temp = new NodeFile(f_name, f_version, f_hash);
		infotbl->InsertFile(f_temp);
	}

	std::cout << "\tLoad socket entity" << std::endl;
	for (nodenum_t it = 0; it < socket_num; it++)
	{
		std::getline(socket_file, line);
		std::istringstream socket(line);
		std::string s_hash_str;
		socket >> s_hash_str;
		hash_t s_hash = std::stol(s_hash_str);

		std::string s_name;
		socket >> s_name;

		NodeSocket *s_temp = new NodeSocket(s_name, s_hash);
		infotbl->InsertSocket(s_temp);
	}

	std::vector<std::string> files = TraverseFile(kg_path + "/");
	int dar_file_num = 0;
	for (auto file : files)
	{
		if (file.find("edgefact") != std::string::npos)
		{
			dar_file_num++;
		}
	}

	// load multiple files to memory
	for (int i = 0; i < dar_file_num; i++)
	{
		std::string edge_path = _edge_path + std::to_string(i) + ".txt";
		std::ifstream edge_file(edge_path, std::ios::in);
		if (!edge_file.is_open())
		{
			std::cerr << "Fail to open file:" << edge_path << std::endl;
		}
		std::getline(edge_file, line);
		nodenum_t edge_num = std::stoi(line);

		std::cout << "\tLoad edges" << std::endl;
		for (nodenum_t it = 0; it < edge_num; it++)
		{
			std::getline(edge_file, line);
			std::istringstream edge(line);
			std::string e_hash_str;
			edge >> e_hash_str;
			hash_t e_hash = std::stol(e_hash_str);

			std::string n1_hash_str;
			edge >> n1_hash_str;
			hash_t n1_hash = std::stol(n1_hash_str);

			std::string n2_hash_str;
			edge >> n2_hash_str;
			hash_t n2_hash = std::stol(n2_hash_str);

			std::string rel_str;
			edge >> rel_str;
			EdgeType_t rel = (EdgeType_t)std::stoi(rel_str);

			std::string seq_str;
			edge >> seq_str;
			seq_t seq = stoint128_t(seq_str);

			std::string sess_str;
			edge >> sess_str;
			sess_t sess = std::stol(sess_str);

			std::string ts;
			edge >> ts;

			KGEdge *e_temp = new KGEdge(n1_hash, n2_hash, rel, seq, sess, e_hash, ts);
			infotbl->InsertEdge(e_temp);
		}
		edge_file.close();
	}

	infotbl->GenKGEdgeTableWhenLoad();

	proc_file.close();
	file_file.close();
	socket_file.close();

	OverheadEnd(start, "Load KG from files");
}

void EntityLoadFromFile(std::string kg_path, KG *infotbl)
{
	auto start = OverheadStart();
	std::cout << "Load system entity facts" << std::endl;
	std::string proc_path = kg_path + "/procfact.txt";
	std::string file_path = kg_path + "/filefact.txt";
	std::string socket_path = kg_path + "/socketfact.txt";

	// read facts
	std::string line;

	std::ifstream proc_file(proc_path, std::ios::in);
	if (!proc_file.is_open())
	{
		std::cerr << "Fail to open file:" << proc_path << std::endl;
		exit(0);
	}
	std::getline(proc_file, line);
	nodenum_t proc_num = std::stoi(line);

	std::ifstream file_file(file_path, std::ios::in);
	if (!file_file.is_open())
	{
		std::cerr << "Fail to open file:" << file_path << std::endl;
	}
	std::getline(file_file, line);
	nodenum_t file_num = std::stoi(line);

	std::ifstream socket_file(socket_path, std::ios::in);
	if (!socket_file.is_open())
	{
		std::cerr << "Fail to open file:" << socket_path << std::endl;
	}
	std::getline(socket_file, line);
	nodenum_t socket_num = std::stoi(line);

	// For Darpa, there should be a diff way to load
	std::cout << "\tLoad process entity" << std::endl;
	for (nodenum_t it = 0; it < proc_num; it++)
	{
		std::getline(proc_file, line);
		std::istringstream proc(line);
		std::string p_hash_str;
		proc >> p_hash_str;
		hash_t p_hash = std::stol(p_hash_str);

		std::string pid;
		proc >> pid;

		std::string exe;
		proc >> exe;

		std::string ppid;
		proc >> ppid;

		std::string args;
		proc >> args;

		NodeProc *p_temp = new NodeProc(pid, exe, args, ppid, p_hash);
		infotbl->InsertProc(p_temp);
	}

	std::cout << "\tLoad file entity" << std::endl;
	for (nodenum_t it = 0; it < file_num; it++)
	{
		std::getline(file_file, line);
		std::istringstream file(line);
		std::string f_hash_str;
		file >> f_hash_str;
		hash_t f_hash = std::stol(f_hash_str);

		std::string f_name;
		file >> f_name;

		std::string f_version;
		file >> f_version;

		NodeFile *f_temp = new NodeFile(f_name, f_version, f_hash);
		infotbl->InsertFile(f_temp);
	}

	std::cout << "\tLoad socket entity" << std::endl;
	for (nodenum_t it = 0; it < socket_num; it++)
	{
		std::getline(socket_file, line);
		std::istringstream socket(line);
		std::string s_hash_str;
		socket >> s_hash_str;
		hash_t s_hash = std::stol(s_hash_str);

		std::string s_name;
		socket >> s_name;

		NodeSocket *s_temp = new NodeSocket(s_name, s_hash);
		infotbl->InsertSocket(s_temp);
	}

	OverheadEnd(start, "Load system entities from files");
}

void LocalStore::SetNodeMap()
{
	int64_t node_id = infotbl->KGNodeTable.size() - 1;
	node_map.clear();
	for (auto node : infotbl->KGNodeTable)
	{
		hash_t id = node.first;
		auto it = node_map.find(id);
		if (it == node_map.end())
		{
			node_map[id] = node_id;
			node_id--;
		}
	}
}

void LocalStore::StoreEntityFile()
{
	SetNodeMap();

	// Writing entity2id.txt file
	int64_t totalNodes = infotbl->KGNodeTable.size();
	std::string entity_file_path = kg_path + "/entity2id.txt";
	std::ofstream entity_file(entity_file_path, std::ios::app);

	if (!entity_file.is_open())
	{
		std::cerr << "Fail to open file:" << entity_file_path << std::endl;
		return;
	}
	entity_file << totalNodes << std::endl;
	for (auto node : node_map)
	{
		entity_file << node.first << " " << node.second << std::endl;
	}

	entity_file.close();
}

void LocalStore::AppendNodeMap(int numExistingNodes)
{
	new_node_map.clear();
	int node_id = numExistingNodes;
	for (auto node : infotbl->KGNodeTable)
	{
		hash_t id = node.first;
		auto it = node_map.find(id);
		if (it == node_map.end()) //if not found in node_map
		{
			node_map[id] = node_id;
			new_node_map[id] = node_id;
			node_id++;
		}
	}
}

void LocalStore::AppendEntityFile()
{
	std::string entity_file_path = kg_path + "/entity2id.txt";
	std::ifstream entity_file(entity_file_path);
	if (!entity_file.is_open())
	{
		std::cerr << "Fail to open file:" << entity_file_path << std::endl;
		return;
	}

	int numExistingNodes;
    if (entity_file >> numExistingNodes) {// The first line was successfully read and converted to an integer.
        hash_t currHash;
        int64_t currEncoding;
		node_map.clear();
        while (entity_file >> currHash >> currEncoding) {// Read and parse the remaining lines
			auto n_it = infotbl->KGNodeTable.find(currHash); 
			if (n_it != infotbl->KGNodeTable.end()) { //if it exists in node table then update nodemap
				node_map[currHash]=currEncoding;
			}
        }
		entity_file.close();
		int newTotalNodes = numExistingNodes+(infotbl->KGNodeTable.size()-node_map.size());
		//now rewrite the first line:
		std::ofstream entity_file_out(entity_file_path);
		if (!entity_file_out.is_open()) {
            std::cerr << "Failed to open file for writing: " << entity_file_path << std::endl;
            return;
        }
        // Write the new first line value.
        entity_file_out << newTotalNodes << std::endl;
		entity_file_out.close();
		AppendNodeMap(numExistingNodes);

		std::ofstream entity_file_out_2(entity_file_path, std::ios::app);	
		if (!entity_file_out_2.is_open()) {
            std::cerr << "Failed to open file for writing in append mode: " << entity_file_path << std::endl;
            return;
        }

		for (auto node : new_node_map) 
		{
			entity_file_out_2 << node.first << " " << node.second << std::endl;
		}
		entity_file_out_2.close();
		new_node_map.clear();

    } else {
        // Failed to read the first line or it was not a valid integer.
        std::cerr << "Failed to read the number of existing nodes from the file." << std::endl;
		entity_file.close();
		StoreEntityFile();
    }
}

void LocalStore::StoreEntityFileChecker()
{
	std::string entity_file_path = kg_path + "/entity2id.txt";
	std::ofstream entity_file(entity_file_path, std::ios::app);

	if (!entity_file.is_open())
	{
		std::cerr << "Fail to open file: " << entity_file_path << std::endl;
		return;
	}

	// Check if the file is empty
	entity_file.seekp(0, std::ios::end);
	if (entity_file.tellp() == 0) //if empty
	{
		entity_file.close(); //close decriptor
		StoreEntityFile();
	}
	else
	{
		entity_file.close(); //close decriptor
		AppendEntityFile();
	}
}


void LocalStore::StoreInteractionFile()
{

	std::string inter_file_path = kg_path + "/inter2id.txt";
	std::ofstream inter_file(inter_file_path, std::ios::app);
	if (!inter_file.is_open())
	{
		std::cerr << "Fail to open file:" << inter_file_path << std::endl;
		return;
	}

	// Writing inter2id.txt file
	for (auto it : infotbl->ObjectInteractionTable)
	{
		try
		{
			inter_file << node_map[it.first] << " ";
			std::vector<hash_t> *interactions = it.second;
			if (interactions == NULL)
			{
				continue;
			}
			for (auto entity : (*interactions))
			{
				inter_file << node_map[entity] << " ";
			}
			inter_file << std::endl;
		}
		catch (...)
		{
			std::cout << "One hot encoding for interaction failed" << std::endl;
		}
	}
	inter_file.close();

	// Adding interactions to train2id.txt file
	std::string train_file_path = kg_path + "/train2id.txt";
	std::ofstream train_file(train_file_path, std::ios::app);
	if (!train_file.is_open())
	{
		std::cerr << "Fail to open file:" << train_file_path << std::endl;
		return;
	}

	for (auto it : infotbl->ObjectInteractionTable)
	{
		try
		{
			// train_file << node_map[it.first] << " ";
			if (it.second == NULL)
				continue;
			for (auto entity : (*it.second))
			{
				train_file << node_map[it.first] << " " << node_map[entity] << " " << 27 << "\n";
				numTrainLines++;
			}
		}
		catch (...)
		{
			std::cout << "One hot encoding for interaction failed at end" << std::endl;
		}
	}
	train_file.close();

	std::string train_file_lines_path = kg_path + "/numTrainLines.txt";
	std::ofstream train_file_lines(train_file_lines_path, std::ios::app);
	if (!train_file_lines.is_open())
	{
		std::cerr << "Fail to open file:" << train_file_path << std::endl;
		return;
	}
	try
	{
		train_file_lines << numTrainLines << "\n";
	}
	catch (...)
	{
		std::cout << "Storing number of lines failed" << std::endl;
	}
}

void LocalStore::StoreRelationFile()
{
	std::string relation_file_path = kg_path + "/relation2id.txt";
	std::ofstream relation_file(relation_file_path, std::ios::app);
	if (!relation_file.is_open())
	{
		std::cerr << "Fail to open file:" << relation_file_path << std::endl;
		return;
	}

	// Writing relation2id.txt file
	relation_file << 28 << std::endl;
	relation_file << "vfork"
				  << " " << 0 << std::endl;
	relation_file << "clone"
				  << " " << 1 << std::endl;
	relation_file << "execve"
				  << " " << 2 << std::endl;
	relation_file << "kill"
				  << " " << 3 << std::endl;
	relation_file << "create"
				  << " " << 4 << std::endl;
	relation_file << "pipe"
				  << " " << 5 << std::endl;
	relation_file << "delete"
				  << " " << 6 << std::endl;
	relation_file << "recv"
				  << " " << 7 << std::endl;
	relation_file << "send"
				  << " " << 8 << std::endl;
	relation_file << "mkdir"
				  << " " << 9 << std::endl;
	relation_file << "rmdir"
				  << " " << 10 << std::endl;
	relation_file << "open"
				  << " " << 11 << std::endl;
	relation_file << "load"
				  << " " << 12 << std::endl;
	relation_file << "read"
				  << " " << 13 << std::endl;
	relation_file << "write"
				  << " " << 14 << std::endl;
	relation_file << "connect"
				  << " " << 15 << std::endl;
	relation_file << "getpeername"
				  << " " << 16 << std::endl;
	relation_file << "filepath"
				  << " " << 17 << std::endl;
	relation_file << "mode"
				  << " " << 18 << std::endl;
	relation_file << "mtime"
				  << " " << 19 << std::endl;
	relation_file << "linknum"
				  << " " << 20 << std::endl;
	relation_file << "uid"
				  << " " << 21 << std::endl;
	relation_file << "count"
				  << " " << 22 << std::endl;
	relation_file << "nametype"
				  << " " << 23 << std::endl;
	relation_file << "version"
				  << " " << 24 << std::endl;
	relation_file << "dev"
				  << " " << 25 << std::endl;
	relation_file << "sizebyte"
				  << " " << 26 << std::endl;
	relation_file << "interaction"
				  << " " << 27 << std::endl;

	relation_file.close();
}

void LocalStore::StoreTrainFile()
{
	SetNodeMap();
	// Writing train2id.txt file
	std::string train_file_path = kg_path + "/train2id.txt";
	std::ofstream train_file(train_file_path, std::ios::app);
	if (!train_file.is_open())
	{
		std::cerr << "Fail to open file:" << train_file_path << std::endl;
		return;
	}

	for (const auto it : infotbl->KGEdgeTable)
	{
		KGEdge *edge = it.second;
		hash_t _n1_hash = edge->n1_hash;
		hash_t _n2_hash = edge->n2_hash;

		std::string parent = std::to_string(node_map[_n1_hash]);
		std::string child = std::to_string(node_map[_n2_hash]);
		std::string relation = std::to_string(EdgeEnum2Int(edge->relation) - 1);
		train_file << parent << " " << child << " " << relation << "\n";

		// new
		numTrainLines++;
	}

	train_file.close();
}

void LocalStore::StoreRecommendationModelFiles()
{
	auto start = OverheadStart();
	std::cout << "Storing nodeHashtoUuid file\n";
	StoreNodeHashToUuidFile();

	std::cout << "Storing entity2id.txt file\n";
	//StoreEntityFile();
	StoreEntityFileChecker();
	std::cout << "Storing relation2id.txt file\n";
	StoreRelationFile();
	std::cout << "Storing inter2id.txt file\n";
	StoreInteractionFile();
	std::cout << "Storing train2id.txt file\n";

	// std::string numTrainLinesStr = std::to_string(numTrainLines);

	// std::string temp_path = kg_path.substr(2); // Dynamic path for storing train2id file
	// std::string bashScript = R"(
	//     #!/bin/bash
	// sed -i "1s/^/$lines\n/" /home/vagrant/ShadeWatcher$temp_path/train2id.txt
	// )";

	// // Execute the Bash script using system()
	// int res1 = setenv("temp_path", temp_path.c_str(), 1);
	// int res2 = setenv("lines", numTrainLinesStr.c_str(), 1);

	// int result = system(bashScript.c_str());

	if (infotbl->malicious_truth)
	{
		std::cout << "Storing maliciousTruth file\n";
		StoreMaliciousTruthFile();
	}

	OverheadEnd(start, "Store Recommendation Model Files");
}

// add the number of processes, files, and sockets in procfact_tmp.txt,
// socketfact_tmp, and filefact_tmp.txt. Write the results back to procfact.txt
// socketfact.txt, and filefact.txt
void LocalStore::DumpProcFileSocketEdge2FactSize(int _file_id)
{
	// record the number of processes
	std::string procfact_path = kg_path + "/procfact.txt";
	std::string procfact_tmp_path = kg_path + "/procfact_tmp.txt";

	std::ofstream procfact_file(procfact_path, std::ios::app);
	if (!procfact_file.is_open())
	{
		std::cerr << "Fail to open file:" << procfact_path << std::endl;
		return;
	}
	std::ifstream procfact_tmp_file(procfact_tmp_path, std::ios::in);
	if (!procfact_tmp_file.is_open())
	{
		std::cerr << "Fail to open file:" << procfact_tmp_path << std::endl;
		return;
	}
	procfact_file << std::to_string(proc_num) << "\n";
	procfact_file << procfact_tmp_file.rdbuf();
	procfact_file.close();
	procfact_tmp_file.close();
	std::remove(procfact_tmp_path.c_str());

	// record the number of files
	std::string filefact_path = kg_path + "/filefact.txt";
	std::string filefact_tmp_path = kg_path + "/filefact_tmp.txt";
	std::ofstream filefact_file(filefact_path, std::ios::app);
	if (!filefact_file.is_open())
	{
		std::cerr << "Fail to open file:" << filefact_path << std::endl;
		return;
	}
	std::ifstream filefact_tmp_file(filefact_tmp_path, std::ios::in);
	if (!filefact_tmp_file.is_open())
	{
		std::cerr << "Fail to open file:" << filefact_tmp_path << std::endl;
		return;
	}
	filefact_file << std::to_string(file_num) << "\n";
	filefact_file << filefact_tmp_file.rdbuf();
	filefact_file.close();
	filefact_tmp_file.close();
	std::remove(filefact_tmp_path.c_str());

	// record the number of sockets
	std::string socketfact_path = kg_path + "/socketfact.txt";
	std::string socketfact_tmp_path = kg_path + "/socketfact_tmp.txt";
	std::ofstream socketfact_file(socketfact_path, std::ios::app);
	if (!socketfact_file.is_open())
	{
		std::cerr << "Fail to open file:" << socketfact_path << std::endl;
		return;
	}
	std::ifstream socketfact_tmp_file(socketfact_tmp_path, std::ios::in);
	if (!socketfact_tmp_file.is_open())
	{
		std::cerr << "Fail to open file:" << socketfact_tmp_path << std::endl;
		return;
	}
	socketfact_file << std::to_string(socket_num) << "\n";
	socketfact_file << socketfact_tmp_file.rdbuf();
	socketfact_file.close();
	socketfact_tmp_file.close();
	std::remove(socketfact_tmp_path.c_str());

	// record the number of all nodes
	std::string nodefact_path = kg_path + "/nodefact.txt";
	std::string nodefact_tmp_path = kg_path + "/nodefact_tmp.txt";
	std::ofstream nodefact_file(nodefact_path, std::ios::app);
	if (!nodefact_file.is_open())
	{
		std::cerr << "Fail to open file:" << nodefact_path << std::endl;
		return;
	}
	std::ifstream nodefact_tmp_file(nodefact_tmp_path, std::ios::in);
	if (!nodefact_tmp_file.is_open())
	{
		std::cerr << "Fail to open file:" << nodefact_tmp_path << std::endl;
		return;
	}
	nodefact_file << std::to_string(node_num) << "\n";
	nodefact_file << nodefact_tmp_file.rdbuf();
	nodefact_file.close();
	nodefact_tmp_file.close();
	std::remove(nodefact_tmp_path.c_str());

	// record the number of edges
	// No edge file is stored
	if (_file_id < 0)
	{
		return;
	}

	std::string file_id = std::to_string(_file_id);
	std::string edgefact_path = kg_path + "/edgefact_" + file_id + ".txt";
	std::string edgefact_tmp_path = kg_path + "/edgefact_tmp_" + file_id + ".txt";
	std::ofstream edgefact_file(edgefact_path, std::ios::app);
	if (!edgefact_file.is_open())
	{
		std::cerr << "Fail to open file:" << edgefact_path << std::endl;
		return;
	}
	std::ifstream edgefact_tmp_file(edgefact_tmp_path, std::ios::in);
	if (!edgefact_tmp_file.is_open())
	{
		std::cerr << "Fail to open file:" << edgefact_tmp_path << std::endl;
		return;
	}
	edgefact_file << std::to_string(edge_num) << "\n";
	edgefact_file << edgefact_tmp_file.rdbuf();
	edgefact_file.close();
	edgefact_tmp_file.close();
	std::remove(edgefact_tmp_path.c_str());
}

void LocalStore::StoreMaliciousTruthFile()
{
	std::string entity_file_path = kg_path + "/maliciousTruth.txt";
	std::ofstream entity_file(entity_file_path, std::ios::app);

	if (!entity_file.is_open())
	{
		std::cerr << "Failed to open file:" << entity_file_path << std::endl;
		return;
	}

	for (auto edge : infotbl->maliciousEdgeTable)
	{
		hash_t subj_id = edge.second->n1_hash;
		hash_t obj_id = edge.second->n2_hash;

		auto subj_it = node_map.find(subj_id);
		auto obj_it = node_map.find(obj_id);

		if (subj_it != node_map.end() && obj_it != node_map.end())
		{ // if both subj and obj exist
			entity_file << subj_it->second << " " << obj_it->second << std::endl;
		}
	}
	entity_file.close();
}

void LocalStore::StoreNodeHashToUuidFile()
{
	std::string entity_file_path = kg_path + "/nodeHashToUuid.txt";
	std::ofstream entity_file(entity_file_path, std::ios::app);
	if (!entity_file.is_open())
	{
		std::cerr << "Failed to open file:" << entity_file_path << std::endl;
		return;
	}

	for (auto n : infotbl->nodeHashToUuid)
	{
		try
		{
			hash_t nodeHash = n.first;
			std::string nodeUuid = n.second;

			entity_file << nodeHash << " " << nodeUuid << "\n";
		}
		catch (...)
		{
			std::cout << "Storing uuid mapping failed" << std::endl;
		}
	}
	entity_file.close();
}