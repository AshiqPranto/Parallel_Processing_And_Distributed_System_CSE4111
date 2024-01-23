#include <bits/stdc++.h>
#include <mpi.h>

using namespace std;

//... To compile: mpic++ phonebook-search.cpp -o phonebook-search
//... To run: mpirun -n 4 ./phonebook-search phonebook1.txt phonebook2.txt

void send_string(string text, int receiver)
{
    int length = text.size() + 1;
    MPI_Send(&length, 1, MPI_INT, receiver, 1, MPI_COMM_WORLD);
    MPI_Send(&text[0], length, MPI_CHAR, receiver, 1, MPI_COMM_WORLD);
}

string receive_string(int sender)
{
    int length;
    MPI_Recv(&length, 1, MPI_INT, sender, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    char *text = new char[length];
    MPI_Recv(text, length, MPI_CHAR, sender, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return string(text);
}

string vector_to_string(vector<string> &words, int start, int end)
{
    string text = "";
    for (int i = start; i < min((int)words.size(), end); i++)
    {
        text += words[i] + "\n";
    }
    return text;
}

vector<string> string_to_vector(string text)
{
    stringstream x(text);
    vector<string> words;
    string word;
    while (x >> word)
    {
        words.push_back(word);
    }
    return words;
}

bool check(string& name, string& number, string& searchName, int rank){
    // if (name.size() != searchName.size()) 
    // {
    //     return false;
    // }
    for (int i = 0; i < searchName.size(); i++)
    {
        if (name[i] != searchName[i])
        {
            return false;
        }
    }
    // cout<<name<<" "<<number<<" found by process "<<rank<<endl;
    // printf("%s %s found by process %d.\n", name.c_str(), number.c_str(), rank);
    return true;
}

void read_phonebook(vector<string> &file_names, vector<string> &names, vector<string> &phone_numbers)
{
    for (auto file_name : file_names)
    {
        ifstream file(file_name);
        string name, number;
        while (file >> name >> number)
        {
            names.push_back(name);
            phone_numbers.push_back(number);
        }
        file.close();
    }
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

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    string search;
    double start_time = MPI_Wtime();

    map<string, string>ultimateResult;


    if (!world_rank)
    {
        vector<string> names, phone_numbers;
        vector<string> file_names(argv + 1, argv + argc);
        read_phonebook(file_names, names, phone_numbers);
        int segment = names.size() / world_size + 1;
        cout << "Give name to search: ";
        cin >> search;
        for (int i = 1; i < world_size; i++)
        {
            int start = i * segment, end = start + segment;
            string names_to_send = vector_to_string(names, start, end);
            send_string(names_to_send, i);
            string phone_numbers_to_send = vector_to_string(phone_numbers, start, end);
            send_string(phone_numbers_to_send, i);
            send_string(search, i);
        }

        for(int i = 0 ; i < segment; i++){
            bool isMatched = check(names[i], phone_numbers[i], search, world_rank);
            if(isMatched){
                ultimateResult[names[i]] = phone_numbers[i];
                // ultimateResult.push_back({names[i], phone_numbers[i]});
            }
        }
    }
    else
    {
        string received_names = receive_string(0);
        vector<string> names = string_to_vector(received_names);
        string received_phone_numbers = receive_string(0);
        vector<string> phone_numbers = string_to_vector(received_phone_numbers);
        search = receive_string(0);

        string matchedName = "";
        string matchedNumber = "";
        for(int i = 0; i < names.size(); i++){
            bool isMatched = check(names[i], phone_numbers[i], search, world_rank);
            if(isMatched){
                matchedName += names[i] + " ";
                matchedNumber += phone_numbers[i] + " ";
            }
        }
        send_string(matchedName, 0);
        send_string(matchedNumber, 0);
    }

    if(world_rank == 0){
        string allMatchedName = "";
        string allMatchedNumber = "";
        for(int i = 1; i < world_size; i++){
            allMatchedName += receive_string(i);
            allMatchedNumber += receive_string(i);
        }
        stringstream ss1(allMatchedName), ss2(allMatchedNumber);
        string word1, word2;
        while(ss1 >> word1 && ss2 >> word2){
            ultimateResult[word1] = word2;
            // ultimateResult.push_back({word1, word2});
        }
        // sort(ultimateResult.begin(), ultimateResult.end());
        for(auto [key, val] : ultimateResult){
            cout<<key<<" -> "<<val<<endl;
        }
    }

    double finish_time = MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD);

    printf("Process %d took %f seconds.\n", world_rank, finish_time - start_time);

    MPI_Finalize();

    return 0;
}