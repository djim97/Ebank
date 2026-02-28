#include"headers.h"

int main()
{
    int choice, subChoice, nb;
    char numero[20];
    float montant;
    do
    {
        fflush(stdin);
        choice = 0;
        subChoice = 0;
        puts("MENU");
        puts("1. Creation compte");
        puts("2. Fermer compte");
        puts("3. Liste comptes");
        puts("4. Liste comptes par clients");
        puts("5. Recherche compte");
        puts("6. Depot");
        puts("7. Retrait");
        puts("8. Liste operations");
        puts("9. Liste operations par compte");
        puts("10. Nombre d'operations");
        puts("11. Quitter");
        do
        {
            puts("Faites un choix !");
            if(scanf("%d",&choice) != 1)
            {
                while(getchar() != '\n');
                choice = 0;
            }
        }while(choice<1 || choice>11);

        switch(choice)
        {
        case 1:
            puts("1. Nouveau client");
            puts("2. Client existant");
            do
            {
                puts("Faites un choix !");
                scanf("%d",&subChoice);
            }while(subChoice<1 || subChoice>2);
            switch(subChoice){
            case 1: {
                Customer newCustomer;
                printf("Nom du client : ");
                scanf(" %44[^\n]", newCustomer.nom);
                printf("Prenom du client : ");
                scanf(" %49[^\n]", newCustomer.prenom);
                do
                {
                    printf("Numero de telephone du client : ");
                    scanf("%d", &newCustomer.phone);
                    if(!phoneValide(newCustomer.phone))
                        puts("Numero de telephone invalide, il doit comporter 9 chiffres et commencer par 77, 78, 75, 76 ou 70");
                } while(!phoneValide(newCustomer.phone));
                Customer *exists = rechercherCustomerParnumero(newCustomer.phone);
                if(exists != NULL)
                {
                    puts("Ce numero de telephone existe deja");
                    free(exists);
                    break;
                }
                saveCustomerInFile(FILE_CUSTOMERS, newCustomer);
                char *numCompte = creerCompte(newCustomer);
                printf("Montant du depot initial : ");
                scanf("%f", &montant);
                depot(numCompte, montant);
                free(numCompte);
                break;

            }


            case 2: {
                int phone;
                printf("Numero de telephone du client : ");
                scanf("%d", &phone);
                Customer *existing = rechercherCustomerParnumero(phone);
                if(existing != NULL)
                {
                    char *numCompte = creerCompte(*existing);
                    printf("Montant du depot initial : ");
                    scanf("%f", &montant);
                    depot(numCompte, montant);
                    free(numCompte);
                    free(existing);
                }

                else
                {
                    puts("Client introuvable");
                }
                break;
            }

        }
            PAUSE
            CLEAR
            break;
        case 2:
            puts("Donner le numero de compte a fermer");
            scanf("%s", numero);
            desactiverCompte(numero);
            PAUSE
            CLEAR
            break;
        case 3:
            puts("Liste des comptes");
            listerComptes();
            PAUSE
            CLEAR
            break;
        case 4: {
            int phone;
            printf("Numero de telephone du client : ");
            scanf("%d", &phone);
            listerComptesParClient(phone);
            PAUSE
            CLEAR
            break;
        }
        case 5:
            puts("Donner le numero de compte a rechercher");
            scanf("%s", numero);
            ACCOUNT *account = rechercherCompteParNumero(numero);
            if(account != NULL)
            {
                puts("Compte Trouve");
                afficherCompte(*account);
                free(account);
            }
            else
            {
                puts("Compte introuvable");
            }
            PAUSE
            CLEAR
            break;
        case 6:
            puts("Donner le numero de compte a deposer");
            scanf("%s", numero);
            do
            {
                puts("Donner le montant a deposer");
                scanf("%f",&montant);
            }
            while(montant<=0);
            depot(numero, montant);
            PAUSE
            CLEAR
            break;
        case 7:
            puts("Donner le numero de compte a retirer");
            scanf("%s", numero);
            do
            {
                puts("Donner le montant a retirer");
                scanf("%f",&montant);
            }
            while(montant<=0);
            retrait(numero, montant);
            PAUSE
            CLEAR
            break;
        case 8:
            puts("Liste des operations");
            listerOperations();
            PAUSE
            CLEAR
            break;
        case 9:
            puts("Donner le numero de compte");
            scanf("%s", numero);
            fflush(stdin);
            {
                ACCOUNT *checkAcc = rechercherCompteParNumero(numero);
                if(checkAcc == NULL)
                {
                    puts("Compte introuvable.");
                    PAUSE
                    CLEAR
                    break;
                }
                free(checkAcc);
            }
            puts("1. Toutes les operations");
            puts("2. Operations DEPOT");
            puts("3. Operations RETRAIT");
            subChoice = 0;
            do
            {
                puts("Faites un choix !");
                if(scanf("%d",&subChoice) != 1)
                {
                    while(getchar() != '\n');
                    subChoice = 0;
                }
            }while(subChoice<1 || subChoice>3);
            if(subChoice == 1)
            {
                listerOperationsParCompte(numero);
                if(nombreOperationsParCompte(numero) > 0)
                {
                    puts("Generer un releve bancaire en PDF ? (1: Oui / 2: Non)");
                    fflush(stdin);
                    subChoice = 0;
                    scanf("%d", &subChoice);
                    if(subChoice == 1)
                        genererRelevePDF(numero, "TOUS");
                }
            }
            else if(subChoice == 2)
            {
                listerOperationsParCompteEtType(numero, "DEPOT");
                if(nombreOperationsParCompteEtType(numero, "DEPOT") > 0)
                {
                    puts("Generer un releve bancaire en PDF ? (1: Oui / 2: Non)");
                    fflush(stdin);
                    subChoice = 0;
                    scanf("%d", &subChoice);
                    if(subChoice == 1)
                        genererRelevePDF(numero, "DEPOT");
                }
            }
            else
            {
                listerOperationsParCompteEtType(numero, "RETRAIT");
                if(nombreOperationsParCompteEtType(numero, "RETRAIT") > 0)
                {
                    puts("Generer un releve bancaire en PDF ? (1: Oui / 2: Non)");
                    fflush(stdin);
                    subChoice = 0;
                    scanf("%d", &subChoice);
                    if(subChoice == 1)
                        genererRelevePDF(numero, "RETRAIT");
                }
            }
            PAUSE
            CLEAR
            break;
        case 10:
            puts("Donner le numero de compte");
            scanf("%s", numero);
            fflush(stdin);
            {
                ACCOUNT *checkAcc = rechercherCompteParNumero(numero);
                if(checkAcc == NULL)
                {
                    puts("Compte introuvable.");
                    PAUSE
                    CLEAR
                    break;
                }
                free(checkAcc);
            }
            puts("1. Toutes les operations");
            puts("2. Operations DEPOT");
            puts("3. Operations RETRAIT");
            subChoice = 0;
            do
            {
                puts("Faites un choix !");
                if(scanf("%d",&subChoice) != 1)
                {
                    while(getchar() != '\n');
                    subChoice = 0;
                }
            }while(subChoice<1 || subChoice>3);
            if(subChoice == 1)
            {
                nb = nombreOperationsParCompte(numero);
                printf("Nombre d'operations pour le compte %s : %d\n", numero, nb);
                if(nb > 0)
                {
                    puts("Generer un resume en PDF ? (1: Oui / 2: Non)");
                    fflush(stdin);
                    subChoice = 0;
                    scanf("%d", &subChoice);
                    if(subChoice == 1)
                        genererResumePDF(numero, "TOUS");
                }
            }
            else if(subChoice == 2)
            {
                nb = nombreOperationsParCompteEtType(numero, "DEPOT");
                printf("Nombre d'operations DEPOT pour le compte %s : %d\n", numero, nb);
                if(nb > 0)
                {
                    puts("Generer un resume en PDF ? (1: Oui / 2: Non)");
                    fflush(stdin);
                    subChoice = 0;
                    scanf("%d", &subChoice);
                    if(subChoice == 1)
                        genererResumePDF(numero, "DEPOT");
                }
            }
            else
            {
                nb = nombreOperationsParCompteEtType(numero, "RETRAIT");
                printf("Nombre d'operations RETRAIT pour le compte %s : %d\n", numero, nb);
                if(nb > 0)
                {
                    puts("Generer un resume en PDF ? (1: Oui / 2: Non)");
                    fflush(stdin);
                    subChoice = 0;
                    scanf("%d", &subChoice);
                    if(subChoice == 1)
                        genererResumePDF(numero, "RETRAIT");
                }
            }
            PAUSE
            CLEAR
            break;
        case 11:
            quitterApplication();
            return 0;
        }
    }
    while(1);
}
