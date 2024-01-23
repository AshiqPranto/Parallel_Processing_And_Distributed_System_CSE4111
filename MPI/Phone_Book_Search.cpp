#include<bits/stdc++.h>
#include<mpi.h>
using namespace std;


void readPhoneBook(vector<string>& fileNames,vector<string>& names,vector<string>& numbers){
    for(auto fileName : fileNames){
        ifstream file(fileName);
        string name, number;
        while(file >> name >> number){
            names.emplace_back(name);
            numbers.emplace_back(number);
            // cout<<"name = "<<name<<", Number = "<<number<<endl;
        }
        file.close();


        // ifstream file(fileName);
        // string line;
        // while(getline(file, line)){
        //     stringstream st(line);
        //     string word;
        //     while(st >> word){
        //         cout<<word<<", ";
        //     }
        //     cout<<endl;
        // }

    }
}
string vectorToString(vector<string>& words, int start, int end){
    string allWords = "";
    for(int i = start; i < min(end, (int)words.size()); i++){
        allWords += words[i] + "\n";
    }
    return allWords;
}
vector<string> stringToVector(string words){
    stringstream st(words);
    vector<string> allWords;
    string word;
    while(st >> word){
        allWords.emplace_back(word);
    }
    return allWords;
}

void sendString(string& text,int receiver){
    int sz = text.size();
    MPI_Send(&sz, 1, MPI_INT, receiver, 0, MPI_COMM_WORLD);
    MPI_Send(&text, sz, MPI_CHAR, receiver, 0, MPI_COMM_WORLD);
}
string receiveString(int sender){
    int sz;
    MPI_Recv(&sz, 1, MPI_INT, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    char *text = new char[sz];
    MPI_Recv(text, sz, MPI_CHAR, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return string(text);
}
void check(string& name, string& number, string& searchName, int rank){
    if (name.size() != searchName.size()) 
    {
        return;
    }
    for (int i = 0; i < searchName.size(); i++)
    {
        if (name[i] != searchName[i])
        {
            return;
        }
    }
    cout<<name<<" "<<number<<" found by process "<<rank<<endl;
    // printf("%s %s found by process %d.\n", name.c_str(), number.c_str(), rank);
}

int main(int argc, char** argv){
    
    MPI_Init(&argc, &argv);

    int rank, total_procs;
    MPI_Comm_size(MPI_COMM_WORLD, &total_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double startTime = MPI_Wtime();

    if(rank == 0){
        vector<string>names, numbers;
        vector<string>fileNames(argv + 1, argv + argc);
        readPhoneBook(fileNames, names, numbers);
        int segment = names.size() / total_procs;
        int start, end;
        string searchName = "Abraham__Ralph";
        cout<<"Enter the name you search: "<<endl;
        cin>>searchName;
        for(int i = 1; i < total_procs; i++){
            start = i * segment, end = start + segment;
            string namesToSend = vectorToString(names, start, end);
            sendString(namesToSend, i);
            string numbersToSend = vectorToString(numbers, start, end);
            sendString(numbersToSend, i);
            sendString(searchName, i);
        }
        for(int i = 0 ; i < segment; i++){
            check(names[i], numbers[i], searchName, rank);
        }
    }
    else{
        string receivedNames = receiveString(0);
        vector<string> names = stringToVector(receivedNames);
        string receivedNumbers = receiveString(0);
        vector<string> numbers = stringToVector(receivedNumbers);
        string searchName = receiveString(0);
        
        for(int i = 0; i < names.size(); i++){
            check(names[i], numbers[i], searchName, rank);
        }
    }
    double finishTime = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);

    printf("Process %d took %f seconds.\n", rank, finishTime - startTime);

    MPI_Finalize();

    return 0;
}