#include"headers.h"

DATETIME getCurrentDateTime()
{
    DATETIME dt;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    dt.jour    = tm.tm_mday;
    dt.mois    = tm.tm_mon + 1;
    dt.annee   = tm.tm_year + 1900;
    dt.heure   = tm.tm_hour;
    dt.minute  = tm.tm_min;
    dt.seconde = tm.tm_sec;

    return dt;
}

void afficherDateTime(DATETIME dt)
{
    printf("%d/%d/%d %c %02d:%02d:%02d\n",
           dt.jour,dt.mois,dt.annee,133,dt.heure,dt.minute,dt.seconde);
}

char *generateNumeroCompte()
{
    char* numero = malloc(20);
    long taille = getFileSize(FILE_ACCOUNTS);
    int nb = (int)taille/sizeof(ACCOUNT);
    nb++;
    sprintf(numero, "CPT-%03d", nb);
    return numero;
}

char *generateNumeroOperation()
{
    char* numero = malloc(20);
    long taille = getFileSize(FILE_OPERATIONS);
    int nb = (int)taille/sizeof(OPERATION);
    nb++;
    sprintf(numero, "OP-%03d", nb);
    return numero;
}

char *creerCompte(Customer customer)
{
    ACCOUNT acc;
    acc.balance = 0.0;
    strcpy(acc.status, "ACTIF");
    acc.createdAt = getCurrentDateTime();
    acc.owner = customer;
    char *numero = generateNumeroCompte();
    strcpy(acc.numero, numero);
    saveAccountInFile(FILE_ACCOUNTS, acc);
    printf("Compte (%s) cree avec succes\n", acc.numero);
    return numero;
}

void listerComptes()
{
    ACCOUNT acc;
    FILE *f = fopen(FILE_ACCOUNTS, "rb");
    rewind(f);
    printf("\n==============================================================================================\n");
    printf("| %-10s | %-19s | %-12s | %-29s | %-8s |\n",
           "NUMERO",
           "DATE CREATION",
           "SOLDE",
           "PROPRIETAIRE",
           "STATUT");
    printf("==============================================================================================\n");
    while(fread(&acc, sizeof(ACCOUNT), 1, f) == 1)
    {
        afficherCompte(acc);
    }
    printf("==============================================================================================\n");
    fclose(f);
}

void listerComptesParClient(int phone)
{
    ACCOUNT acc;
    FILE *f = fopen(FILE_ACCOUNTS, "rb");
    int trouve = 0;
    if(f == NULL)
    {
        puts("Aucun compte enregistre.");
        return;
    }
    printf("\n==============================================================================================\n");
    printf("| %-10s | %-19s | %-12s | %-29s | %-8s |\n",
           "NUMERO",
           "DATE CREATION",
           "SOLDE",
           "PROPRIETAIRE",
           "STATUT");
    printf("==============================================================================================\n");
    while(fread(&acc, sizeof(ACCOUNT), 1, f) == 1)
    {
        if(acc.owner.phone == phone)
        {
            afficherCompte(acc);
            trouve = 1;
        }
    }
    printf("==============================================================================================\n");
    if(!trouve)
        puts("Aucun compte trouve pour ce client.");
    fclose(f);
}

ACCOUNT *rechercherCompteParNumero(char numeroCompte[])
{
    FILE *f = fopen(FILE_ACCOUNTS, "rb");
    ACCOUNT *acc = malloc(sizeof(ACCOUNT));
    if (f == NULL)
    {
        return NULL;
    }
    while (fread(acc, sizeof(ACCOUNT), 1, f) == 1)
    {
        if (strcasecmp(numeroCompte, acc->numero) == 0)
        {
            fclose(f);
            return acc;
        }
    }
    fclose(f);
    free(acc);
    return NULL;
}


void depot(char numeroCompte[], float amount)
{
    FILE *f = fopen(FILE_ACCOUNTS, "rb+");
    ACCOUNT acc;
    int trouve = 0;
    while(fread(&acc, sizeof(ACCOUNT), 1, f) == 1)
    {
        if(strcasecmp(numeroCompte, acc.numero) == 0)
        {
            if(strcmp(acc.status, "INACTIF") == 0)
            {
                puts("Ce compte est desactive. Operation impossible.");
                trouve = 1;
                break;
            }
            acc.balance += amount;
            fseek(f, -(long)sizeof(ACCOUNT), SEEK_CUR);
            fwrite(&acc, sizeof(ACCOUNT), 1, f);
            printf("Depot de %.2f effectue sur le compte %s.\n", amount, numeroCompte);
            OPERATION op;
            op.account = acc;
            op.amount = amount;
            strcpy(op.numero, generateNumeroOperation());
            op.operationDate = getCurrentDateTime();
            strcpy(op.type, "DEPOT");
            saveOperationInFile(FILE_OPERATIONS, op);
            trouve = 1;
            break;
        }
    }
    if(!trouve)
    {
        printf("Compte %s introuvable.\n", numeroCompte);
    }
    fclose(f);
}

void retrait(char numeroCompte[], float montant)
{
    FILE *f = fopen(FILE_ACCOUNTS, "rb+");
    ACCOUNT acc;
    int trouve = 0;
    while(fread(&acc, sizeof(ACCOUNT), 1, f) == 1)
    {
        if(strcasecmp(numeroCompte, acc.numero) == 0)
        {
            if(strcmp(acc.status, "INACTIF") == 0)
            {
                puts("Ce compte est desactive. Operation impossible.");
                trouve = 1;
                break;
            }
            if(acc.balance > montant)
            {
                acc.balance -= montant;
                fseek(f, -(long)sizeof(ACCOUNT), SEEK_CUR);
                fwrite(&acc, sizeof(ACCOUNT), 1, f);
                printf("Retrait de %.2f effectue sur le compte %s.\n", montant, numeroCompte);
                OPERATION op;
                op.account = acc;
                op.amount = montant;
                strcpy(op.numero, generateNumeroOperation());
                op.operationDate = getCurrentDateTime();
                strcpy(op.type, "RETRAIT");
                saveOperationInFile(FILE_OPERATIONS, op);
            }
            else
            {
                puts("Solde insuffisant.");
            }
            trouve = 1;
            break;
        }
    }
    if(!trouve)
    {
        printf("Compte %s introuvable.\n", numeroCompte);
    }
    fclose(f);
}

void listerOperations()
{
    OPERATION op;
    FILE *f = fopen(FILE_OPERATIONS, "rb");
    rewind(f);
    printf("\n========================================================================================================\n");
    printf("| %-10s | %-19s | %-12s | %-30s | %-17s |\n",
           "NUMERO",
           "DATE OPERATION",
           "MONTANT",
           "NUMERO COMPTE",
           "TYPE OPERATION");
    printf("========================================================================================================\n");
    while(fread(&op, sizeof(OPERATION), 1, f) == 1)
    {
        afficherOperation(op);
    }
    printf("========================================================================================================\n");
    fclose(f);
}

void listerOperationsParCompte(char numeroCompte[])
{
    OPERATION op;
    FILE *f = fopen(FILE_OPERATIONS, "rb");
    int trouve = 0;
    if(f == NULL)
    {
        puts("Aucune operation enregistree.");
        return;
    }
    printf("\n========================================================================================================\n");
    printf("| %-10s | %-19s | %-12s | %-30s | %-17s |\n",
           "NUMERO",
           "DATE OPERATION",
           "MONTANT",
           "NUMERO COMPTE",
           "TYPE OPERATION");
    printf("========================================================================================================\n");
    while(fread(&op, sizeof(OPERATION), 1, f) == 1)
    {
        if(strcasecmp(numeroCompte, op.account.numero) == 0)
        {
            afficherOperation(op);
            trouve = 1;
        }
    }
    printf("========================================================================================================\n");
    if(!trouve)
        printf("Aucune operation trouvee pour le compte %s.\n", numeroCompte);
    fclose(f);
}

void listerOperationsParCompteEtType(char numeroCompte[], char type[])
{
    OPERATION op;
    FILE *f = fopen(FILE_OPERATIONS, "rb");
    int trouve = 0;
    if(f == NULL)
    {
        puts("Aucune operation enregistree.");
        return;
    }
    printf("\n========================================================================================================\n");
    printf("| %-10s | %-19s | %-12s | %-30s | %-17s |\n",
           "NUMERO",
           "DATE OPERATION",
           "MONTANT",
           "NUMERO COMPTE",
           "TYPE OPERATION");
    printf("========================================================================================================\n");
    while(fread(&op, sizeof(OPERATION), 1, f) == 1)
    {
        if(strcasecmp(numeroCompte, op.account.numero) == 0 && strcasecmp(type, op.type) == 0)
        {
            afficherOperation(op);
            trouve = 1;
        }
    }
    printf("========================================================================================================\n");
    if(!trouve)
        printf("Aucune operation %s trouvee pour le compte %s.\n", type, numeroCompte);
    fclose(f);
}

void listerOperationsParType(char type[])
{
    OPERATION op;
    FILE *f = fopen(FILE_OPERATIONS, "rb");
    int trouve = 0;
    if(f == NULL)
    {
        puts("Aucune operation enregistree.");
        return;
    }
    printf("\n========================================================================================================\n");
    printf("| %-10s | %-19s | %-12s | %-30s | %-17s |\n",
           "NUMERO",
           "DATE OPERATION",
           "MONTANT",
           "NUMERO COMPTE",
           "TYPE OPERATION");
    printf("========================================================================================================\n");
    while(fread(&op, sizeof(OPERATION), 1, f) == 1)
    {
        if(strcasecmp(type, op.type) == 0)
        {
            afficherOperation(op);
            trouve = 1;
        }
    }
    printf("========================================================================================================\n");
    if(!trouve)
        printf("Aucune operation de type %s trouvee.\n", type);
    fclose(f);
}

void desactiverCompte(char numeroCompte[])
{
    FILE *f = fopen(FILE_ACCOUNTS, "rb+");
    ACCOUNT acc;
    int trouve = 0;
    while(fread(&acc, sizeof(ACCOUNT), 1, f) == 1)
    {
        if(strcasecmp(numeroCompte, acc.numero) == 0)
        {
            if(strcmp(acc.status, "INACTIF") == 0)
            {
                puts("Ce compte est deja desactive.");
                trouve = 1;
                break;
            }
            if(acc.balance > 0)
            {
                OPERATION op;
                op.account = acc;
                op.amount = acc.balance;
                strcpy(op.numero, generateNumeroOperation());
                op.operationDate = getCurrentDateTime();
                strcpy(op.type, "RETRAIT");
                saveOperationInFile(FILE_OPERATIONS, op);
                printf("Retrait de %.2f effectue sur le compte %s.\n", acc.balance, numeroCompte);
            }
            acc.balance = 0;
            strcpy(acc.status, "INACTIF");
            fseek(f, -(long)sizeof(ACCOUNT), SEEK_CUR);
            fwrite(&acc, sizeof(ACCOUNT), 1, f);
            printf("Compte %s desactive avec succes.\n", numeroCompte);
            trouve = 1;
            break;
        }
    }
    if(!trouve)
    {
        printf("Compte %s introuvable.\n", numeroCompte);
    }
    fclose(f);
}

static void writePDF(const char *filename, const char *content, int contentLen)
{
    FILE *f = fopen(filename, "wb");
    if(f == NULL)
    {
        puts("Erreur creation fichier PDF.");
        return;
    }

    long offsets[6];

    fprintf(f, "%%PDF-1.4\n");

    offsets[1] = ftell(f);
    fprintf(f, "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n");

    offsets[2] = ftell(f);
    fprintf(f, "2 0 obj\n<< /Type /Pages /Kids [3 0 R] /Count 1 >>\nendobj\n");

    offsets[3] = ftell(f);
    fprintf(f, "3 0 obj\n<< /Type /Page /Parent 2 0 R /MediaBox [0 0 612 792] /Contents 5 0 R /Resources << /Font << /F1 4 0 R >> >> >>\nendobj\n");

    offsets[4] = ftell(f);
    fprintf(f, "4 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Courier >>\nendobj\n");

    offsets[5] = ftell(f);
    fprintf(f, "5 0 obj\n<< /Length %d >>\nstream\n", contentLen);
    fwrite(content, 1, contentLen, f);
    fprintf(f, "\nendstream\nendobj\n");

    long xrefPos = ftell(f);
    fprintf(f, "xref\n0 6\n");
    fprintf(f, "0000000000 65535 f \n");
    for(int i = 1; i <= 5; i++)
        fprintf(f, "%010ld 00000 n \n", offsets[i]);

    fprintf(f, "trailer\n<< /Size 6 /Root 1 0 R >>\nstartxref\n%ld\n%%%%EOF\n", xrefPos);
    fclose(f);
}

static int writePDFAccountHeader(char *buf, ACCOUNT *acc, const char *title)
{
    char fullName[100];
    sprintf(fullName, "%s %s", acc->owner.prenom, acc->owner.nom);

    int len = 0;
    len += sprintf(buf + len, "BT\n");
    len += sprintf(buf + len, "/F1 18 Tf\n");
    len += sprintf(buf + len, "50 750 Td\n");
    len += sprintf(buf + len, "(%s) Tj\n", title);

    len += sprintf(buf + len, "/F1 11 Tf\n");
    len += sprintf(buf + len, "0 -30 Td\n");
    len += sprintf(buf + len, "(Compte    : %s) Tj\n", acc->numero);
    len += sprintf(buf + len, "0 -18 Td\n");
    len += sprintf(buf + len, "(Client    : %s) Tj\n", fullName);
    len += sprintf(buf + len, "0 -18 Td\n");
    len += sprintf(buf + len, "(Telephone : %d) Tj\n", acc->owner.phone);
    len += sprintf(buf + len, "0 -18 Td\n");
    len += sprintf(buf + len, "(Solde     : %.2f FCFA) Tj\n", acc->balance);
    len += sprintf(buf + len, "0 -18 Td\n");
    len += sprintf(buf + len, "(Statut    : %s) Tj\n", acc->status);
    return len;
}

void genererRelevePDF(char numeroCompte[], char type[])
{
    ACCOUNT *acc = rechercherCompteParNumero(numeroCompte);
    if(acc == NULL)
    {
        puts("Compte introuvable.");
        return;
    }

    char filename[100];
    if(strcmp(type, "TOUS") == 0)
        sprintf(filename, "releve_%s.pdf", numeroCompte);
    else
        sprintf(filename, "releve_%s_%s.pdf", numeroCompte, type);

    DATETIME dt = getCurrentDateTime();
    char content[8192];
    int len = writePDFAccountHeader(content, acc, "RELEVE BANCAIRE");

    len += sprintf(content + len, "0 -35 Td\n");
    len += sprintf(content + len, "/F1 14 Tf\n");
    if(strcmp(type, "TOUS") == 0)
        len += sprintf(content + len, "(HISTORIQUE DES OPERATIONS) Tj\n");
    else
        len += sprintf(content + len, "(HISTORIQUE DES OPERATIONS %s) Tj\n", type);
    len += sprintf(content + len, "/F1 9 Tf\n");
    len += sprintf(content + len, "0 -22 Td\n");
    len += sprintf(content + len, "(%-12s %-20s %12s   %-10s) Tj\n", "Numero", "Date", "Montant", "Type");
    len += sprintf(content + len, "0 -5 Td\n");
    len += sprintf(content + len, "(--------------------------------------------------------------) Tj\n");

    float total = 0;
    FILE *fop = fopen(FILE_OPERATIONS, "rb");
    if(fop != NULL)
    {
        OPERATION op;
        while(fread(&op, sizeof(OPERATION), 1, fop) == 1)
        {
            if(strcasecmp(numeroCompte, op.account.numero) == 0 &&
               (strcmp(type, "TOUS") == 0 || strcasecmp(type, op.type) == 0))
            {
                char dateBuf[25];
                sprintf(dateBuf, "%02d/%02d/%04d %02d:%02d",
                    op.operationDate.jour, op.operationDate.mois, op.operationDate.annee,
                    op.operationDate.heure, op.operationDate.minute);
                len += sprintf(content + len, "0 -14 Td\n");
                len += sprintf(content + len, "(%-12s %-20s %12.2f   %-10s) Tj\n",
                    op.numero, dateBuf, op.amount, op.type);
                total += op.amount;
            }
        }
        fclose(fop);
    }

    len += sprintf(content + len, "0 -5 Td\n");
    len += sprintf(content + len, "(--------------------------------------------------------------) Tj\n");
    len += sprintf(content + len, "/F1 11 Tf\n");
    len += sprintf(content + len, "0 -18 Td\n");
    if(strcmp(type, "TOUS") == 0)
        len += sprintf(content + len, "(TOTAL                                    : %.2f FCFA) Tj\n", total);
    else
        len += sprintf(content + len, "(TOTAL %s                              : %.2f FCFA) Tj\n", type, total);

    len += sprintf(content + len, "0 -30 Td\n");
    len += sprintf(content + len, "/F1 8 Tf\n");
    len += sprintf(content + len, "(Document genere le %02d/%02d/%04d a %02d:%02d:%02d) Tj\n",
        dt.jour, dt.mois, dt.annee, dt.heure, dt.minute, dt.seconde);
    len += sprintf(content + len, "ET\n");

    writePDF(filename, content, len);
    free(acc);
    printf("Releve bancaire genere : %s\n", filename);
}

void genererResumePDF(char numeroCompte[], char type[])
{
    ACCOUNT *acc = rechercherCompteParNumero(numeroCompte);
    if(acc == NULL)
    {
        puts("Compte introuvable.");
        return;
    }

    char filename[100];
    if(strcmp(type, "TOUS") == 0)
        sprintf(filename, "resume_%s.pdf", numeroCompte);
    else
        sprintf(filename, "resume_%s_%s.pdf", numeroCompte, type);

    DATETIME dt = getCurrentDateTime();

    int nbTotal = 0, nbDepot = 0, nbRetrait = 0;
    float totalDepot = 0, totalRetrait = 0;
    FILE *fop = fopen(FILE_OPERATIONS, "rb");
    if(fop != NULL)
    {
        OPERATION op;
        while(fread(&op, sizeof(OPERATION), 1, fop) == 1)
        {
            if(strcasecmp(numeroCompte, op.account.numero) == 0)
            {
                nbTotal++;
                if(strcasecmp(op.type, "DEPOT") == 0)
                {
                    nbDepot++;
                    totalDepot += op.amount;
                }
                else if(strcasecmp(op.type, "RETRAIT") == 0)
                {
                    nbRetrait++;
                    totalRetrait += op.amount;
                }
            }
        }
        fclose(fop);
    }

    char content[4096];
    int len = writePDFAccountHeader(content, acc, "RESUME DES OPERATIONS");

    len += sprintf(content + len, "0 -35 Td\n");
    len += sprintf(content + len, "/F1 14 Tf\n");
    if(strcmp(type, "TOUS") == 0)
        len += sprintf(content + len, "(STATISTIQUES) Tj\n");
    else
        len += sprintf(content + len, "(STATISTIQUES - OPERATIONS %s) Tj\n", type);

    len += sprintf(content + len, "/F1 11 Tf\n");
    len += sprintf(content + len, "0 -25 Td\n");
    len += sprintf(content + len, "(--------------------------------------------------------------) Tj\n");

    if(strcmp(type, "TOUS") == 0)
    {
        len += sprintf(content + len, "0 -20 Td\n");
        len += sprintf(content + len, "(Nombre total d'operations    : %d) Tj\n", nbTotal);
        len += sprintf(content + len, "0 -20 Td\n");
        len += sprintf(content + len, "(Nombre d'operations DEPOT    : %d) Tj\n", nbDepot);
        len += sprintf(content + len, "0 -18 Td\n");
        len += sprintf(content + len, "(Montant total DEPOT          : %.2f FCFA) Tj\n", totalDepot);
        len += sprintf(content + len, "0 -20 Td\n");
        len += sprintf(content + len, "(Nombre d'operations RETRAIT  : %d) Tj\n", nbRetrait);
        len += sprintf(content + len, "0 -18 Td\n");
        len += sprintf(content + len, "(Montant total RETRAIT        : %.2f FCFA) Tj\n", totalRetrait);
    }
    else if(strcmp(type, "DEPOT") == 0)
    {
        len += sprintf(content + len, "0 -20 Td\n");
        len += sprintf(content + len, "(Nombre d'operations DEPOT    : %d) Tj\n", nbDepot);
        len += sprintf(content + len, "0 -18 Td\n");
        len += sprintf(content + len, "(Montant total DEPOT          : %.2f FCFA) Tj\n", totalDepot);
    }
    else
    {
        len += sprintf(content + len, "0 -20 Td\n");
        len += sprintf(content + len, "(Nombre d'operations RETRAIT  : %d) Tj\n", nbRetrait);
        len += sprintf(content + len, "0 -18 Td\n");
        len += sprintf(content + len, "(Montant total RETRAIT        : %.2f FCFA) Tj\n", totalRetrait);
    }

    len += sprintf(content + len, "0 -20 Td\n");
    len += sprintf(content + len, "(--------------------------------------------------------------) Tj\n");
    len += sprintf(content + len, "0 -30 Td\n");
    len += sprintf(content + len, "/F1 8 Tf\n");
    len += sprintf(content + len, "(Document genere le %02d/%02d/%04d a %02d:%02d:%02d) Tj\n",
        dt.jour, dt.mois, dt.annee, dt.heure, dt.minute, dt.seconde);
    len += sprintf(content + len, "ET\n");

    writePDF(filename, content, len);
    free(acc);
    printf("Resume genere : %s\n", filename);
}

int nombreOperationsParCompte(char numeroCompte[])
{
    OPERATION op;
    FILE *f = fopen(FILE_OPERATIONS, "rb");
    int nb = 0;
    if(f == NULL)
        return 0;
    while(fread(&op, sizeof(OPERATION), 1, f) == 1)
    {
        if(strcasecmp(numeroCompte, op.account.numero) == 0)
            nb++;
    }
    fclose(f);
    return nb;
}

int nombreOperationsParCompteEtType(char numeroCompte[], char type[])
{
    OPERATION op;
    FILE *f = fopen(FILE_OPERATIONS, "rb");
    int nb = 0;
    if(f == NULL)
        return 0;
    while(fread(&op, sizeof(OPERATION), 1, f) == 1)
    {
        if(strcasecmp(numeroCompte, op.account.numero) == 0 && strcasecmp(type, op.type) == 0)
            nb++;
    }
    fclose(f);
    return nb;
}

int nombreComptes()
{
    long taille = getFileSize(FILE_ACCOUNTS);
    return (int)taille/sizeof(ACCOUNT);
}

int quitterApplication()
{
    puts("Bye Bye !");
    return 0;
}

long getFileSize(char filename[])
{
    FILE *f = fopen(filename, "rb");
    long taille = 0;
    if (f == NULL)
    {
        return 0;
    }
    fseek(f, 0, SEEK_END);
    taille = ftell(f);
    fclose(f);
    return taille;
}

void saveAccountInFile(char filename[], ACCOUNT acc)
{
    FILE *f = fopen(filename, "ab");
    fwrite(&acc, sizeof(ACCOUNT), 1, f);
    fclose(f);
}

void saveCustomerInFile(char filename[], Customer customer)
{
    FILE *f = fopen(filename, "ab");
    fwrite(&customer, sizeof(Customer), 1, f);
    fclose(f);
}

Customer *rechercherCustomerParnumero(int phone)
{
    FILE *f = fopen(FILE_CUSTOMERS, "rb");
    Customer *c = malloc(sizeof(Customer));
    if(f == NULL)
    {
        free(c);
        return NULL;
    }
    while(fread(c, sizeof(Customer), 1, f) == 1)
    {
        if(c->phone == phone)
        {
            fclose(f);
            return c;
        }
    }
    fclose(f);
    free(c);
    return NULL;
}


void saveOperationInFile(char filename[], OPERATION op)
{
    FILE *f = fopen(filename, "ab");
    fwrite(&op, sizeof(OPERATION), 1, f);
    fclose(f);
}

void afficherCompte(ACCOUNT acc)
{
    char fullName[100];
    sprintf(fullName, "%s %s", acc.owner.prenom, acc.owner.nom);
    printf("| %-10s | %02d/%02d/%04d %02d:%02d:%02d | %-12.2f | %-29s | %-8s |\n",
           acc.numero,
           acc.createdAt.jour,
           acc.createdAt.mois,
           acc.createdAt.annee,
           acc.createdAt.heure,
           acc.createdAt.minute,
           acc.createdAt.seconde,
           acc.balance,
           fullName,
           acc.status);
}

void afficherOperation(OPERATION op) {
    printf("| %-10.10s | %02d/%02d/%04d %02d:%02d:%02d | %-12.2f | %-30s | %-17s |\n",
           op.numero,
           op.operationDate.jour,
           op.operationDate.mois,
           op.operationDate.annee,
           op.operationDate.heure,
           op.operationDate.minute,
           op.operationDate.seconde,
           op.amount,
           op.account.numero,
           op.type);
}
int phoneValide(int phone)
{
    if(phone < 100000000 || phone > 999999999)
        return 0;
    int prefix = phone / 10000000;
    if(prefix == 77 || prefix == 78 || prefix == 75 || prefix == 76 || prefix == 70)
        return 1;
    return 0;
}


