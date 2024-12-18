#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <map>
#include <unordered_set>

// Simple structure for Commit
struct Commit {
    std::string commitHash;
    std::string message;
    std::string timestamp;
    std::string changes; // Simple change description
    std::string branchName; // Branch this commit belongs to

    Commit(std::string msg, std::string changes, std::string branch)
        : message(msg), changes(changes), branchName(branch) {
        // Generate timestamp for commit
        time_t now = time(0);
        timestamp = ctime(&now);

        // Simple hash for commit (could be a proper hash like SHA1, for now just using timestamp)
        commitHash = std::to_string(std::hash<std::string>{}(timestamp + message));
    }
};

// Repository manager class
class RepoManager {
private:
    std::map<std::string, std::vector<Commit>> branches; // Branches and their commits
    std::string currentBranch = "main";  // Default branch
    std::unordered_set<std::string> files; // Set of files in the repo
    std::string repoDirectory;

    // Utility function to generate commit message from modified files
    std::string generateCommitMessage(const std::vector<std::string>& modifiedFiles) {
        std::stringstream message;
        message << "Modified files: ";
        for (const auto& file : modifiedFiles) {
            message << file << " ";
        }
        return message.str();
    }

    // Utility function to join a list of strings with commas
    std::string join(const std::vector<std::string>& list, const std::string& delimiter) {
        std::stringstream ss;
        for (size_t i = 0; i < list.size(); ++i) {
            ss << list[i];
            if (i != list.size() - 1) ss << delimiter;
        }
        return ss.str();
    }

    // Simple conflict detection between two sets of changes (just for demonstration)
    bool hasConflict(const std::vector<std::string>& changes1, const std::vector<std::string>& changes2) {
        std::unordered_set<std::string> set1(changes1.begin(), changes1.end());
        std::unordered_set<std::string> set2(changes2.begin(), changes2.end());

        for (const auto& change : changes1) {
            if (set2.find(change) != set2.end()) {
                return true; // Conflict found
            }
        }
        return false;
    }

public:
    RepoManager() : repoDirectory(".cbird") {
        if (!std::filesystem::exists(repoDirectory)) {
            std::filesystem::create_directory(repoDirectory);
        }

        // Create a default 'main' branch
        branches["main"] = std::vector<Commit>();
    }

    void initRepo() {
        if (std::filesystem::exists(".cbird")) {
            std::cerr << "Error: Repository already initialized!" << std::endl;
            return;
        }

        std::ofstream cbirdFile(".cbird");
        if (cbirdFile.is_open()) {
            cbirdFile << "CodeBird Repository\n";
            cbirdFile.close();
            std::cout << "Repository initialized! .cbird file created." << std::endl;
        } else {
            std::cerr << "Error: Failed to create .cbird file!" << std::endl;
        }
    }

    void addFile(std::string filename) {
        files.insert(filename);
        std::cout << "File added: " << filename << std::endl;
    }

    void commitChanges(std::vector<std::string> modifiedFiles) {
        if (modifiedFiles.empty()) {
            std::cerr << "Error: No files modified to commit." << std::endl;
            return;
        }

        std::string message = generateCommitMessage(modifiedFiles);
        Commit newCommit(message, "Modified " + join(modifiedFiles, ", "), currentBranch);
        branches[currentBranch].push_back(newCommit);

        std::cout << "Commit made on branch " << currentBranch << " with message: " << message << std::endl;
    }

    void showCommitHistory() {
        std::cout << "Commit History for branch " << currentBranch << ":\n";
        for (auto& commit : branches[currentBranch]) {
            std::cout << "Commit Hash: " << commit.commitHash << "\n";
            std::cout << "Message: " << commit.message << "\n";
            std::cout << "Timestamp: " << commit.timestamp;
            std::cout << "Changes: " << commit.changes << "\n\n";
        }
    }

    void showStatus() {
        std::cout << "Currently on branch: " << currentBranch << std::endl;
    }

    void createBranch(std::string branchName) {
        if (branches.find(branchName) != branches.end()) {
            std::cerr << "Error: Branch already exists!" << std::endl;
            return;
        }
        branches[branchName] = std::vector<Commit>();
        std::cout << "Branch " << branchName << " created." << std::endl;
    }

    void switchBranch(std::string branchName) {
        if (branches.find(branchName) == branches.end()) {
            std::cerr << "Error: Branch does not exist!" << std::endl;
            return;
        }
        currentBranch = branchName;
        std::cout << "Switched to branch " << branchName << std::endl;
    }

    void mergeBranch(std::string branchName) {
        if (branches.find(branchName) == branches.end()) {
            std::cerr << "Error: Branch does not exist!" << std::endl;
            return;
        }

        std::cout << "Merging branch " << branchName << " into " << currentBranch << std::endl;

        std::vector<std::string> changesCurrentBranch;
        std::vector<std::string> changesOtherBranch;

        // Collect the changes (for simplicity, let's assume each commit has a simple list of changed files)
        for (const auto& commit : branches[currentBranch]) {
            changesCurrentBranch.push_back(commit.changes);
        }

        for (const auto& commit : branches[branchName]) {
            changesOtherBranch.push_back(commit.changes);
        }

        // Check for conflicts
        if (hasConflict(changesCurrentBranch, changesOtherBranch)) {
            std::cout << "Conflict detected! Merge cannot be completed automatically." << std::endl;
            std::cout << "Please resolve conflicts manually in the following files: ";
            for (const auto& file : files) {
                std::cout << file << " ";
            }
            std::cout << "\nMerge aborted." << std::endl;
            return;
        }

        // If no conflicts, perform the merge (basic logic: append commits from the other branch)
        branches[currentBranch].insert(branches[currentBranch].end(),
                                       branches[branchName].begin(), branches[branchName].end());

        std::cout << "Merge completed successfully!" << std::endl;
    }

    // Help function to show available commands
    void showHelp() {
        std::cout << "CodeBird - A simple version control system\n\n";
        std::cout << "Usage:\n";
        std::cout << "  codebird <command> <repo_name> [options]\n\n";
        std::cout << "Commands:\n";
        std::cout << "  init                  Initialize a new CodeBird repository\n";
        std::cout << "  add <file>            Add a file to the repository\n";
        std::cout << "  commit <file>         Commit changes made to the repository\n";
        std::cout << "  log                   Show the commit history of the current branch\n";
        std::cout << "  status                Show the current status of the repository\n";
        std::cout << "  create <branch_name>  Create a new branch\n";
        std::cout << "  switch <branch_name>  Switch to an existing branch\n";
        std::cout << "  merge <branch_name>   Merge a branch into the current branch\n";
        std::cout << "  --help, -h            Show this help message\n";
        std::cout << "\nFor more information, see the CodeBird documentation.\n";
    }
};

// Function to handle the CLI commands
void handleCLI(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: codebird <command> <repo_name> [options]" << std::endl;
        return;
    }

    std::string command = argv[1];

    // If the user requests help
    if (command == "--help" || command == "-h") {
        RepoManager repo;
        repo.showHelp();
        return;
    }

    std::string repoName = argv[2];
    RepoManager repo;

    if (command == "init") {
        repo.initRepo();
    } else if (command == "add") {
        if (argc < 4) {
            std::cerr << "Error: No file specified to add." << std::endl;
            return;
        }
        std::string file = argv[3];
        repo.addFile(file);
    } else if (command == "commit") {
        if (argc < 4) {
            std::cerr << "Error: No file specified for commit." << std::endl;
            return;
        }
        std::string file = argv[3];
        repo.commitChanges({file});
    } else if (command == "log") {
        repo.showCommitHistory();
    } else if (command == "status") {
        repo.showStatus();
    } else if (command == "create") {
        if (argc < 4) {
            std::cerr << "Error: No branch name specified." << std::endl;
            return;
        }
        std::string branchName = argv[3];
        repo.createBranch(branchName);
    } else if (command == "switch") {
        if (argc < 4) {
            std::cerr << "Error: No branch name specified." << std::endl;
            return;
        }
        std::string branchName = argv[3];
        repo.switchBranch(branchName);
    } else if (command == "merge") {
        if (argc < 4) {
            std::cerr << "Error: No branch name specified to merge." << std::endl;
            return;
        }
        std::string branchName = argv[3];
        repo.mergeBranch(branchName);
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
    }
}

int main(int argc, char** argv) {
    handleCLI(argc, argv);
    return 0;
}
