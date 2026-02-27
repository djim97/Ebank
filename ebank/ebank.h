typedef struct
{
    int jour;
    int mois;
    int annee;
    int heure;
    int minute;
    int seconde;
} DATETIME;


typedef struct
{
    char nom[45];
    char prenom[50];
    int phone;
} Customer;


typedef struct
{
    char numero[45];
    Customer owner;
    float balance;
    char status[10];
    DATETIME createdAt;
} ACCOUNT;


typedef struct
{
    char numero[45];
    char type[50];
    float amount;
    ACCOUNT account;
    DATETIME operationDate;
} OPERATION;

DATETIME getCurrentDateTime();
void afficherDateTime(DATETIME dt);
char *generateNumeroCompte();
char *generateNumeroOperation();
char *creerCompte(Customer customer);
void listerComptes();
void saveCustomerInFile(char filename[], Customer customer);
Customer *rechercherCustomerParnumero(int phone);
ACCOUNT *rechercherCompteParNumero(char numeroCompte[]);
void depot(char[], float);
void retrait(char numeroCompte[], float montant);
void listerOperations();
int nombreComptes();
int quitterApplication();
long getFileSize(char[]);
void saveAccountInFile(char[], ACCOUNT);
void saveOperationInFile(char[], OPERATION);
void afficherCompte(ACCOUNT);
void afficherOperation(OPERATION);
void desactiverCompte(char numeroCompte[]);
void listerComptesParClient(int phone);
void listerOperationsParCompte(char numeroCompte[]);
void listerOperationsParCompteEtType(char numeroCompte[], char type[]);
void listerOperationsParType(char type[]);
void genererRelevePDF(char numeroCompte[], char type[]);
void genererResumePDF(char numeroCompte[], char type[]);
int nombreOperationsParCompte(char numeroCompte[]);
int nombreOperationsParCompteEtType(char numeroCompte[], char type[]);
int phoneValide(int phone);
