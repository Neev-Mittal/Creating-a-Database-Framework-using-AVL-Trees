#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// User profile structure
struct Profile {
    int id;
    char name[100];
    int age;
    char gender; // 'M' for Male, 'F' for Female
    char contact[15];
};

// Node structure with height
struct Node {
    struct Profile profile;
    int height;
    struct Node *left;
    struct Node *right;
};

// Function to create a new node
struct Node* createNode(int id, const char* name, int age, char gender, const char* contact) {
    struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->profile.id = id;
    strcpy(newNode->profile.name, name);
    newNode->profile.age = age;
    newNode->profile.gender = gender;
    strcpy(newNode->profile.contact, contact);
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->height = 1; // New node is initially added at height 1
    return newNode;
}

// Function to get the height of a node
int height(struct Node *node) {
    if (node == NULL)
        return 0;
    return node->height;
}

// Function to get the maximum of two integers
int maximum(int a, int b) {
    return (a > b) ? a : b;
}

// Function to calculate the balance factor of a node
int getBalanceFactor(struct Node *node) {
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}

// Right rotation
struct Node* rightRotate(struct Node *y) {
    struct Node *x = y->left;
    struct Node *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = maximum(height(y->left), height(y->right)) + 1;
    x->height = maximum(height(x->left), height(x->right)) + 1;

    // Return new root
    return x;
}

// Left rotation
struct Node* leftRotate(struct Node *x) {
    struct Node *y = x->right;
    struct Node *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = maximum(height(x->left), height(x->right)) + 1;
    y->height = maximum(height(y->left), height(y->right)) + 1;

    // Return new root
    return y;
}

// Function to find the node with the minimum value in a subtree
struct Node* minValueNode(struct Node* node) {
    struct Node* current = node;

    // Loop down to find the leftmost leaf
    while (current && current->left != NULL)
        current = current->left;

    return current;
}

// Recursive function to insert a node and balance the AVL tree
struct Node* insert(struct Node* node, int id, const char* name, int age, char gender, const char* contact) {
    // Step 1: Perform the normal BST insertion
    if (node == NULL)
        return createNode(id, name, age, gender, contact);

    if (id < node->profile.id)
        node->left = insert(node->left, id, name, age, gender, contact);
    else if (id > node->profile.id)
        node->right = insert(node->right, id, name, age, gender, contact);
    else // Equal IDs are not allowed
        return node;

    // Step 2: Update height of the ancestor node
    node->height = 1 + maximum(height(node->left), height(node->right));

    // Step 3: Get the balance factor of the node
    int balance = getBalanceFactor(node);

    // Step 4: Check the four balance cases

    // Left Left Case
    if (balance > 1 && id < node->left->profile.id)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && id > node->right->profile.id)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && id > node->left->profile.id) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && id < node->right->profile.id) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    // Return the (unchanged) node pointer
    return node;
}

// Function to delete a node with a given key from the AVL tree
struct Node* deleteNode(struct Node* root, int id) {
    // Step 1: Perform standard BST deletion
    if (root == NULL)
        return root;

    // If the key to be deleted is smaller than the root's key,
    // it lies in the left subtree
    if (id < root->profile.id)
        root->left = deleteNode(root->left, id);

    // If the key to be deleted is greater than the root's key,
    // it lies in the right subtree
    else if (id > root->profile.id)
        root->right = deleteNode(root->right, id);

    // If the key is the same as the root's key, this is the node to be deleted
    else {
        // Node with only one child or no child
        if ((root->left == NULL) || (root->right == NULL)) {
            struct Node* temp = root->left ? root->left : root->right;

            // No child case
            if (temp == NULL) {
                temp = root;
                root = NULL;
            }
            else { // One child case
                *root = *temp; // Copy the contents of the non-empty child
            }

            free(temp);
        }
        else {
            // Node with two children: Get the in-order successor (smallest in the right subtree)
            struct Node* temp = minValueNode(root->right);

            // Copy the in-order successor's data to this node
            root->profile = temp->profile;

            // Delete the in-order successor
            root->right = deleteNode(root->right, temp->profile.id);
        }
    }

    // If the tree had only one node then return
    if (root == NULL)
        return root;

    // Step 2: Update the height of the current node
    root->height = 1 + maximum(height(root->left), height(root->right));

    // Step 3: Get the balance factor of this node to check whether
    // this node became unbalanced
    int balance = getBalanceFactor(root);

    // Step 4: If the node is unbalanced, then try out the four cases

    // Left Left Case
    if (balance > 1 && getBalanceFactor(root->left) >= 0)
        return rightRotate(root);

    // Left Right Case
    if (balance > 1 && getBalanceFactor(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // Right Right Case
    if (balance < -1 && getBalanceFactor(root->right) <= 0)
        return leftRotate(root);

    // Right Left Case
    if (balance < -1 && getBalanceFactor(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    // Return the (unchanged) node pointer
    return root;
}

// Function to search for a profile by ID
struct Node* search(struct Node* root, int id) {
    // Base case: root is null or the key is present at the root
    if (root == NULL || root->profile.id == id)
        return root;

    // Key is greater than the root's key
    if (id > root->profile.id)
        return search(root->right, id);

    // Key is smaller than the root's key
    return search(root->left, id);
}

// Function for pre-order traversal of the tree
void preOrder(struct Node *root) {
    if (root != NULL) {
        printf("ID: %d, Name: %s, Age: %d, Gender: %c, Contact: %s\n",
               root->profile.id, root->profile.name,
               root->profile.age, root->profile.gender,
               root->profile.contact);
        preOrder(root->left);
        preOrder(root->right);
    }
}

// Function for in-order traversal of the tree
void inOrder(struct Node *root) {
    if (root != NULL) {
        inOrder(root->left);
        printf("ID: %d, Name: %s, Age: %d, Gender: %c, Contact: %s\n",
               root->profile.id, root->profile.name,
               root->profile.age, root->profile.gender,
               root->profile.contact);
        inOrder(root->right);
    }
}

// Function for post-order traversal of the tree
void postOrder(struct Node *root) {
    if (root != NULL) {
        postOrder(root->left);
        postOrder(root->right);
        printf("ID: %d, Name: %s, Age: %d, Gender: %c, Contact: %s\n",
               root->profile.id, root->profile.name,
               root->profile.age, root->profile.gender,
               root->profile.contact);
    }
}

// Main function
int main() {
    struct Node *head = NULL;
    int id, age, choice;
    char name[100], gender, contact[15];

    do {
        printf("\nHuman Database Operations:\n");
        printf("1. Add User Profile\n");
        printf("2. Delete User Profile\n");
        printf("3. Search User Profile\n");
        printf("4. Preorder Traversal\n");
        printf("5. Inorder Traversal\n");
        printf("6. Postorder Traversal\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                printf("Enter the User ID:\n");
                scanf("%d", &id);
                printf("Enter the User Name:\n");
                scanf(" %[^\n]", name);
                printf("Enter the User Age:\n");
                scanf("%d", &age);
                printf("Enter the User Gender (M/F):\n");
                scanf(" %c", &gender);  // Note the space before %c to consume the newline
                printf("Enter the Contact Number:\n");
                scanf(" %[^\n]", contact);
                head = insert(head, id, name, age, gender, contact);
                printf("User profile added successfully!\n");
                break;
            case 2:
                printf("Enter the User ID to delete:\n");
                scanf("%d", &id);
                head = deleteNode(head, id);
                printf("User profile deleted successfully!\n");
                break;
            case 3: {
                printf("Enter the User ID to search:\n");
                scanf("%d", &id);
                struct Node* result = search(head, id);
                if (result != NULL) {
                    printf("Profile Found:\n");
                    printf("ID: %d, Name: %s, Age: %d, Gender: %c, Contact: %s\n",
                           result->profile.id, result->profile.name,
                           result->profile.age, result->profile.gender,
                           result->profile.contact);
                } else {
                    printf("Profile with ID %d not found.\n", id);
                }
                break;
            }
            case 4:
                printf("Preorder Traversal:\n");
                preOrder(head);
                break;
            case 5:
                printf("Inorder Traversal:\n");
                inOrder(head);
                break;
            case 6:
                printf("Postorder Traversal:\n");
                postOrder(head);
                break;
            case 7:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice, please try again.\n");
                break;
        }
    } while (choice != 7);

    return 0;
}
