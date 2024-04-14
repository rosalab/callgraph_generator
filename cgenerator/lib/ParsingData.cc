#include<iostream>
#include<cstring>
#include<fstream>

using namespace std;

string FilePath = "../../indirectcall-analysis/target_bpf_indir.log";

int main(void){
	
	ifstream ifs;

	ifs.open(FilePath, ifstream::in);
	
	string read_line;

	while(ifs.good()){
		getline(ifs, read_line);
		if(strstr(read_line.c_str(), "[CALLER]")){
			cout<<read_line<<endl;
			char *dup = strdup(read_line.c_str());
			char *pch = strtok(dup, ":");
			while(pch !=NULL){
				// Insert in a Map
				//cout<<pch<<endl;
				pch = strtok(NULL, ":");
			}
			getline(ifs, read_line);
			dup = strdup(read_line.c_str());
			pch = strtok(dup, ":");
			int i = 0;
			while(pch !=NULL){
				// Insert in a Map
				if(i == 1){
					i = stoi(pch);
					break;
				}
				//cout<<pch<<endl;
				pch = strtok(NULL, ":");
				i++;
			}
			for(int j = 0; j < i; j++){
				getline(ifs, read_line);
				cout<<read_line<<endl;
				if(read_line.empty()) break;
				dup = strdup(read_line.c_str());
				pch = strtok(dup, " ");
				while(pch !=NULL){
					cout<<pch<<endl;
					pch = strtok(NULL, " ");
				}
			}
		}
	}

	ifs.close();
	return 0;
	
}
