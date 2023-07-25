#include "driverdar.h"

// new -- Helper function to check if a file exists
bool fileExists(const std::string &filePath)
{
	return boost::filesystem::exists(filePath);
}

// new --
std::unordered_map<std::string, std::string> CollectMaliciousTruthFiles(
	const std::vector<std::string> &darpa_files,
	const std::string &malicious_truth_path)
{
	std::unordered_map<std::string, std::string> malicious_files_map;

	for (const std::string &darpa_file : darpa_files)
	{
		// Extract the file name from the path
		boost::filesystem::path darpa_path(darpa_file);
		std::string darpa_file_name = darpa_path.filename().string();

		// Form the corresponding malicious_truth file name
		std::string malicious_truth_file_name = darpa_file_name + "_malicious_truth.txt";
		std::string malicious_truth_file_path = malicious_truth_path + "/" + malicious_truth_file_name;

		// Check if the corresponding malicious_truth file exists
		if (fileExists(malicious_truth_file_path))
		{
			malicious_files_map[darpa_file] = malicious_truth_file_path;
		}
	}

	return malicious_files_map;
}

int main(int argc, char **argv)
{

	// parse command line arguments
	Config cfg(argc, argv);
	cfg.ConfigDar();

	// Knowledge graph for darpa
	KG *infotbl = new KG(darpa);

	// define Local File in KG (no computation)
	LocalStore ls(cfg.embed_data_path, infotbl);
	// visulize in neo4j
	Neo4jdb neo4jdb(cfg.neo4j_config, infotbl);

	// KG Construction
	// 1. load kg from db/file
	// 2. parse logs to construct kg
	if (cfg.loadentity)
	{
		// load system entities from local files
		EntityLoadFromFile(cfg.embed_data_path, infotbl);
		// print KG information
		infotbl->PrintKG();
	}

	if (cfg.loadfromdb)
	{
		// load knowledge graph from database
		if (cfg.ld_all_edg_flag == true)
		{
			KGLoadAllFromDB(cfg.postgres_config, infotbl, cfg.darpa_dataset);
		}
		else
		{
			KGLoadPartialFromDB(cfg.postgres_config, infotbl, cfg.darpa_dataset, cfg.edg_ld_files);
		}
		// print KG information
		infotbl->PrintKG();
	}
	else if (cfg.loadfromfile)
	{
		// load knowledge graph from local files
		KGLoadFromFile(cfg.embed_data_path, infotbl);
		// print KG information
		infotbl->PrintKG();
	}
	else
	{
		// collect darpa files under darpa_data_dir
		auto darpa_files = CollectJsonFile(cfg.darpa_data_dir);

		// new
		std::unordered_map<std::string, std::string> malicious_truth_files;
		if (cfg.malicious_truth)
		{
			malicious_truth_files = CollectMaliciousTruthFiles(darpa_files, cfg.malicious_truth_path);
		}
		// -- -- --

		int file_id = 0;
		for (auto darpa_file : darpa_files)
		{
			// new
			if (cfg.malicious_truth)
			{
				auto malPath = malicious_truth_files.find(darpa_file);
				if (malPath != malicious_truth_files.end())
				{
					appendMaliciousInteractions(malPath->second, infotbl);
				}
			}
			// ----

			// construct KG (work_threads shows #threads to parse dataset)
			// std::cout << "TABLE SIZE1: " << infotbl->nodeHashToUuid.size() << "," << infotbl->ObjectInteractionTable.size() << std::endl;
			KGConstruction(darpa_file, infotbl, cfg);
			// std::cout << "TABLE SIZE2: " << infotbl->nodeHashToUuid.size() << "," << infotbl->ObjectInteractionTable.size() << std::endl;

			// print KG information
			infotbl->PrintKG();

			// store kg in local file or database
			// TODO: specify the file id to insert edge.
			if (cfg.storetodb)
			{
				KGStoreToDB(cfg.postgres_config, infotbl, cfg.darpa_dataset, file_id);
				file_id += 1;
			}

			if (cfg.storetofile)
			{
				ls.KGStoreToFile(file_id);
				file_id += 1;
			}

			if (cfg.storeentity)
			{
				ls.EntityStoreToFile();
			}
			// Appending train2id.txt file for each file to keep edges which are otherwise removed by free interaction
			ls.StoreTrainFile();

			// ************ visualize attacks in darpa dataset *******************
			// ******************************************************************

			// to save memory, we clean up KG after parsing a darpa file
			infotbl->FreeInteraction();
		}
		ls.StoreRecommendationModelFiles();

		if (cfg.storetofile || cfg.storeentity)
		{
			ls.DumpProcFileSocketEdge2FactSize(file_id - 1);
		}
	}

	infotbl->FreeInteraction();
	infotbl->FreeNode();
	delete (infotbl);
	return 0;
}
