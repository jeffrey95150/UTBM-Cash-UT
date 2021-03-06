#include <my_global.h>
#include <mysql.h>

#include <glib.h>

#include "main_mysql.h"
#include "main_callbacks.h"

#include "structures.h"


extern MYSQL *con;

extern const gchar 	*database_host;
extern const gchar 	*database_db;
extern const gchar 	*database_user;
extern const gchar 	*database_pass;

extern LISTEUTILISATEURS *data_utilisateurs;
extern Produit *data_recherche_produits;


/*** INTERFACES ***/
void finish_with_error()
{
	fprintf(stderr, "%s\n", mysql_error(con));
	mysql_close(con);       
}


void connect_to_mysql()
{
	con = mysql_init(NULL);

	if (con == NULL) {
		dialog_mysql_error();
	}

	if (mysql_real_connect(con, database_host, database_user, database_pass, database_db, 0, NULL, 0) == NULL) 
	{
		dialog_mysql_error();
		mysql_close(con);
	} else {
		mysql_query(con, "SET NAMES 'utf8'"); /* Debug pour accents */
	}

}

/*** GESTION CONNEXION ***/

int mysql_verifier_identification(const gchar *utilisateur, const gchar *motdepasse){

	gchar *query = NULL;

	query = g_strconcat("SELECT * FROM utilisateurs WHERE nom=\"", utilisateur, "\" AND motdepasse=\"", motdepasse, "\"", NULL);

	if (con == NULL) {
		g_print("Database error.\n");
		exit(1);
	} else {

		if (mysql_query(con, (char *)query) ) 
		{
			finish_with_error(con);
		}
		MYSQL_RES *result = mysql_store_result(con);

		if (result == NULL) {
			finish_with_error(con);
		}

		int num_rows;
		num_rows = mysql_num_rows(result);


		if( num_rows == 1 ){
			return 1;
		} else {
			return 0;
		}

		mysql_free_result(result);
	}


}


const gchar *mysql_niveau_utilisateur(const gchar *utilisateur, const gchar *motdepasse){

	gchar *query = NULL;

	query = g_strconcat("SELECT * FROM utilisateurs WHERE nom=\"", utilisateur, "\" AND motdepasse=\"", motdepasse, "\"", NULL);


	if (con == NULL) {
		g_print("Database error.\n");
		exit(1);
	} else {

		if (mysql_query(con, (char *)query) ) 
		{
			finish_with_error(con);
		}
		MYSQL_RES *result = mysql_store_result(con);

		if (result == NULL) {
			finish_with_error(con);
		}

		int num_rows;
		num_rows = mysql_num_rows(result);

		const gchar *niveau;
		niveau = "1";

		if( num_rows == 1 ){
			MYSQL_ROW row;
			while( (row = mysql_fetch_row(result)) ){ 
				niveau = row[1];
			}
		} else {
			exit(1);
		}

		return niveau;
		mysql_free_result(result);
	}

}

const gchar *mysql_num_utilisateur(const gchar *utilisateur, const gchar *motdepasse){

	gchar *query = NULL;

	query = g_strconcat("SELECT * FROM utilisateurs WHERE nom=\"", utilisateur, "\" AND motdepasse=\"", motdepasse, "\"", NULL);


	if (con == NULL) {
		g_print("Database error.\n");
		exit(1);
	} else {

		if (mysql_query(con, (char *)query) ) 
		{
			finish_with_error(con);
		}
		MYSQL_RES *result = mysql_store_result(con);

		if (result == NULL) {
			finish_with_error(con);
		}

		int num_rows;
		num_rows = mysql_num_rows(result);

		const gchar *num;
		num = "0";

		if( num_rows == 1 ){
			MYSQL_ROW row;
			while( (row = mysql_fetch_row(result)) ){ 
				num = row[0];
			}
		}

		return num;
		mysql_free_result(result);
	}

}

/*** PRODUITS ***/

int mysql_verifier_existence_produit(const gchar *code_barres){

	gchar *query = NULL;
	query = g_strconcat("SELECT * FROM produits WHERE ean13=\"", code_barres, "\"", NULL);

	if (con == NULL) {
		g_print("Database error.\n");
		exit(1);
	} else {

		if (mysql_query(con, (char *)query) ) 
		{
			finish_with_error(con);
		}
		MYSQL_RES *result = mysql_store_result(con);

		if (result == NULL) {
			finish_with_error(con);
		}

		int num_rows;
		num_rows = mysql_num_rows(result);

		if( num_rows == 1 ){
			return 1;
		} else {
			return 0;
		}

		mysql_free_result(result);
	}


}

struct Produit *mysql_recuperer_produit(const gchar *code_barres){

	gchar *query = NULL;
	query = g_strconcat("SELECT * FROM produits WHERE ean13=\"", code_barres, "\"", NULL);

	Produit *mon_produit = malloc(sizeof(*mon_produit)); /* crée un pointeur de la taille de la structure */

	if (con == NULL) {
		g_print("Database error.\n");
		exit(1);
	} else {

		if (mysql_query(con, (char *)query) ) 
		{
			finish_with_error(con);
		}
		MYSQL_RES *result = mysql_store_result(con);

		if (result == NULL) {
			finish_with_error(con);
		}

		int num_rows;
		num_rows = mysql_num_rows(result);

		
		if( num_rows == 1 ){
			MYSQL_ROW row;
			while( (row = mysql_fetch_row(result)) ){ 

				/* remplie les valeurs de la structure */
				mon_produit->produitid = atoi(row[0]);
				mon_produit->code_barres =  g_strconcat(row[1], NULL);
				mon_produit->marque =  g_strconcat(row[2], NULL);
				mon_produit->libelle = g_strconcat(row[3], NULL);
				mon_produit->type_marque =  g_strconcat(row[4], NULL);
				mon_produit->conditionnement =  g_strconcat(row[5], NULL);
				mon_produit->code_TVA = atoi(row[6]);
				mon_produit->prix = atof(row[7]);

			}
		} else {
			/* remplie les valeurs de la structure */
			mon_produit->produitid = 0;
			mon_produit->code_barres = "0000000000000";
			mon_produit->marque = "0";
			mon_produit->libelle = "0";
			mon_produit->type_marque = "0";
			mon_produit->code_TVA = 0;
			mon_produit->prix = 0.00;
		}

		mysql_free_result(result);
	}

	return mon_produit; // pour retourner le pointeur

}



int mysql_produit_ajouter(const gchar *code_barres, const gchar *marque, const gchar *libelle, const gchar *type_marque, const gchar *conditionnement, const gchar *code_tva, const gchar *prix){


	/*char *c_code_tva;
	sprintf(c_code_tva, "%d", code_tva);

	char *cPrix_produit;
	sprintf(cPrix_produit, "%.2f", prix_produit);*/

	gchar *query = NULL;
	query = g_strconcat("INSERT INTO produits (ean13, marque, libelle, type_marque, conditionnement, code_tva, prix) VALUES(\"", code_barres ,"\",\"", marque,"\",\"", libelle,"\",\"", type_marque,"\",\"", conditionnement,"\",\"", code_tva,"\",\"", prix,"\")", NULL);

	if (con == NULL) {
		g_print("Database error.\n");
		return 0;
	} else if ( mysql_query(con, (char *)query) ){
		finish_with_error(con);
		return 0;
	} else {
		return 1;
	}

}


int mysql_produit_modifier(const gchar *code_barres, const gchar *marque, const gchar *libelle, const gchar *type_marque, const gchar *conditionnement, const gchar *code_tva, const gchar *prix){


	gchar *query = NULL;
	query = g_strconcat("UPDATE produits SET ean13=\"", code_barres ,"\", marque=\"", marque,"\", libelle=\"", libelle,"\", type_marque=\"", type_marque,"\", conditionnement=\"", conditionnement,"\", code_tva=\"", code_tva,"\", prix=\"", prix,"\" WHERE ean13=\"", code_barres ,"\"", NULL);


	if (con == NULL) {
		g_print("Database error.\n");
		return 0;
	} else if ( mysql_query(con, (char *)query) ){
		finish_with_error(con);
		return 0;
	} else {
		return 1;
	}

}

int mysql_produit_supprimer(const gchar *code_barres){

	gchar *query = NULL;
	query = g_strconcat("DELETE FROM produits WHERE ean13=\"", code_barres ,"\"", NULL);

	if (con == NULL) {
		g_print("Database error.\n");
		return 0;
	} else if ( mysql_query(con, (char *)query) ){
		finish_with_error(con);
		return 0;
	} else {
		return 1;
	}

}




int mysql_nombre_rechercher_produits(const gchar *critere, const gchar *recherche){
	
	gchar *query = NULL;
	query = g_strconcat("SELECT * FROM produits WHERE ", critere, " LIKE \"%", recherche, "%\"", NULL);

	if (con == NULL) {
		g_print("Database error.\n");
		exit(1);
	} else {

		if (mysql_query(con, (char *)query) ) 
		{
			finish_with_error(con);
		}

		MYSQL_RES *result = mysql_store_result(con);

		if (result == NULL) {
			finish_with_error(con);
		}

		int num_rows;
		num_rows = mysql_num_rows(result);

		return num_rows;
		mysql_free_result(result);
	}

}


Produit *mysql_rechercher_produits(const gchar *critere, const gchar *recherche){

	gchar *query = NULL;
	query = g_strconcat("SELECT * FROM produits WHERE ", critere, " LIKE \"%", recherche, "%\"", NULL);

	if (con == NULL) {
		g_print("Database error.\n");
		exit(1);
	} else {

		if (mysql_query(con, (char *)query) ) 
		{
			finish_with_error(con);
		}

		MYSQL_RES *result = mysql_store_result(con);

		if (result == NULL) {
			finish_with_error(con);
		}

		int num_rows;
		num_rows = mysql_num_rows(result);

		/* crée un pointeur de la taille de la structure multiplié par le nombre de produits trouvés */
		data_recherche_produits = (Produit *) malloc(sizeof(Produit) * num_rows);

		MYSQL_ROW row;
		gint i;
		i = 0;

		while( (row = mysql_fetch_row(result)) ){ 
			/* remplie les valeurs de la structure */
			data_recherche_produits[i].produitid = atoi(row[0]);
			data_recherche_produits[i].code_barres = g_strconcat((const gchar *)row[1], NULL);
			data_recherche_produits[i].marque = g_strconcat((const gchar *)row[2], NULL);
			data_recherche_produits[i].libelle = g_strconcat((const gchar *)row[3], NULL);
			data_recherche_produits[i].type_marque = g_strconcat((const gchar *)row[4], NULL);
			data_recherche_produits[i].conditionnement = g_strconcat((const gchar *)row[5], NULL);
			data_recherche_produits[i].code_TVA = atoi(row[6]);
			data_recherche_produits[i].prix = atof(g_strconcat((const gchar *)row[7], NULL));

			i++;
		}

		mysql_free_result(result);
		return data_recherche_produits;
	}

}


/**** TAUX TVA ****/

int mysql_nombre_taux_tva(){
	
	gchar *query = NULL;
	query = g_strconcat("SELECT * FROM taux_tva", NULL);

	if (con == NULL) {
		g_print("Database error.\n");
		exit(1);
	} else {

		if (mysql_query(con, (char *)query) ) 
		{
			finish_with_error(con);
		}
		MYSQL_RES *result = mysql_store_result(con);

		if (result == NULL) {
			finish_with_error(con);
		}

		int num_rows;
		num_rows = mysql_num_rows(result);

		return num_rows;
		mysql_free_result(result);

	}

}


TauxTVA *mysql_recuperer_taux_tva(){
	//struct TauxTVA;

	gchar *query = NULL;
	query = g_strconcat("SELECT * FROM taux_tva", NULL);

	if (con == NULL) {
		g_print("Database error.\n");
		exit(1);
	} else {

		if (mysql_query(con, (char *)query) ) 
		{
			finish_with_error(con);
		}
		MYSQL_RES *result = mysql_store_result(con);

		if (result == NULL) {
			finish_with_error(con);
		}

		int num_rows;
		num_rows = mysql_num_rows(result);

		
		TauxTVA *codes_tauxtva = malloc(sizeof(TauxTVA) * num_rows); /* crée un pointeur de la taille de la structure multiplié par le nombre de taux dans la base */

		MYSQL_ROW row;
		int i;
		i = 0;

		while( (row = mysql_fetch_row(result)) ){ 
			/* remplie les valeurs de la structure */
			codes_tauxtva[i].code = atoi(row[0]);
			codes_tauxtva[i].taux = atof(row[1]);
			codes_tauxtva[i].ht = 0.0;
			codes_tauxtva[i].tva = 0.0;
			codes_tauxtva[i].ttc = 0.0;
			i++;
		}
	
		return codes_tauxtva;
		mysql_free_result(result);
	}

}


/**** GESTION UTILISATEURS ****/

int mysql_nombre_utilisateurs(){
	
	gchar *query = NULL;
	query = g_strconcat("SELECT * FROM utilisateurs", NULL);

	if (con == NULL) {
		g_print("Database error.\n");
		exit(1);
	} else {

		if (mysql_query(con, (char *)query) ) 
		{
			finish_with_error(con);
		}

		MYSQL_RES *result = mysql_store_result(con);

		if (result == NULL) {
			finish_with_error(con);
		}

		int num_rows;
		num_rows = mysql_num_rows(result);

		return num_rows;
		mysql_free_result(result);
	}

}


LISTEUTILISATEURS *mysql_recuperer_utilisateurs(){
	//struct TauxTVA;

	gchar *query = NULL;
	query = g_strconcat("SELECT * FROM utilisateurs", NULL);

	if (con == NULL) {
		g_print("Database error.\n");
		exit(1);
	} else {

		if (mysql_query(con, (char *)query) ) 
		{
			finish_with_error(con);
		}

		MYSQL_RES *result = mysql_store_result(con);

		if (result == NULL) {
			finish_with_error(con);
		}

		int num_rows;
		num_rows = mysql_num_rows(result);

		/* crée un pointeur de la taille de la structure multiplié par le nombre d'utilisateurs dans la base */
		data_utilisateurs = (LISTEUTILISATEURS *) malloc(sizeof(LISTEUTILISATEURS) * num_rows);

		MYSQL_ROW row;
		gint i;
		i = 0;

		while( (row = mysql_fetch_row(result)) ){ 
			/* remplie les valeurs de la structure */
			data_utilisateurs[i].id = g_strconcat((const gchar *)row[0], NULL);
			data_utilisateurs[i].utilisateur = g_strconcat((const gchar *)row[2], NULL);
			data_utilisateurs[i].motdepasse = g_strconcat((const gchar *)row[3], NULL);
			data_utilisateurs[i].niveau = g_strconcat((const gchar *)row[1], NULL);

			i++;
		}

		mysql_free_result(result);
		return data_utilisateurs;
	}

}


int mysql_utilisateur_ajouter(const gchar *utilisateur, const gchar *motdepasse, const gchar *niveau){

	gchar *query = NULL;
	query = g_strconcat("INSERT INTO utilisateurs (niveau, nom, motdepasse) VALUES(\"", niveau ,"\",\"", utilisateur,"\",\"", motdepasse,"\")", NULL);

	if (con == NULL) {
		g_print("Database error.\n");
		return 0;
	} else if ( mysql_query(con, (char *)query) ){
		finish_with_error(con);
		return 0;
	} else {
		return 1;
	}

}

int mysql_utilisateur_supprimer(const gchar *id_utilisateur){

	gchar *query = NULL;
	query = g_strconcat("DELETE FROM utilisateurs WHERE id=\"", id_utilisateur ,"\"", NULL);

	if (con == NULL) {
		g_print("Database error.\n");
		return 0;
	} else if ( mysql_query(con, (char *)query) ){
		finish_with_error(con);
		return 0;
	} else {
		return 1;
	}

}

int mysql_utilisateur_modifier(const gchar *id_utilisateur, const gchar *nom_utilisateur, const gchar *mot_de_passe, const gchar *niveau){

	gchar *query = NULL;
	query = g_strconcat("UPDATE utilisateurs SET nom=\"", nom_utilisateur,"\", motdepasse=\"", mot_de_passe,"\", niveau=\"", niveau,"\" WHERE id=\"", id_utilisateur,"\"", NULL);

	if (con == NULL) {
		g_print("Database error.\n");
		return 0;
	} else if ( mysql_query(con, (char *)query) ){
		finish_with_error(con);
		return 0;
	} else {
		return 1;
	}

}


/*** ENCAISSEMENT ***/
int mysql_encaissement_facturer(const gchar *num_caisse, const gchar *num_utilisateur, const gchar *facture, float total_prix){

	char *cTotal_prix;
	sprintf(cTotal_prix, "%.2f", total_prix);

	gchar *query = NULL;
	query = g_strconcat("INSERT INTO factures (date, num_caisse, num_caissiere, facture, total_prix) VALUES(CONCAT(CURDATE(), " ", CURTIME()),\"", num_caisse,"\",\"", num_utilisateur, "\",\"", facture, "\",\"", cTotal_prix, "\")", NULL);

	if (con == NULL) {
		g_print("Database error.\n");
		return 0;
	} else if ( mysql_query(con, (char *)query) ){
		finish_with_error(con);
		return 0;
	} else {
		return 1;
	}

}


/*** STATISTIQUES ***/
int mysql_nombre_ventes_aujourdhui(const gchar *num_utilisateur){ // ID ou "0" pour tous les utilisateurs

	gchar *query = NULL;

	if( g_strcmp0(num_utilisateur, "0") == 0 ){
		query = g_strconcat("SELECT * FROM factures WHERE date >= CURDATE()", NULL);
	} else {
		query = g_strconcat("SELECT * FROM factures WHERE date >= CURDATE() AND num_caissiere=\"", num_utilisateur,"\"", NULL);
	}

	if (con == NULL) {
		g_print("Database error.\n");
		exit(1);
	} else {

		if (mysql_query(con, (char *)query) ) 
		{
			finish_with_error(con);
		}

		MYSQL_RES *result = mysql_store_result(con);

		if (result == NULL) {
			finish_with_error(con);
		}

		int num_rows;
		num_rows = mysql_num_rows(result);

		return num_rows;
		mysql_free_result(result);
	}

}


float mysql_total_encaisse_aujourdhui(const gchar *num_utilisateur){ // ID ou "0" pour tous les utilisateurs
	gchar *query = NULL;

	if( g_strcmp0(num_utilisateur, "0") == 0 ){
		query = g_strconcat("SELECT ROUND(SUM(total_prix), 2) AS somme FROM factures WHERE date >= CURDATE()", NULL);
	} else {
		query = g_strconcat("SELECT ROUND(SUM(total_prix), 2) AS somme FROM factures WHERE date >= CURDATE() AND num_caissiere=\"", num_utilisateur,"\"", NULL);
	}

	if (con == NULL) {
		g_print("Database error.\n");
		exit(1);
	} else {

		if (mysql_query(con, (char *)query) ) 
		{
			finish_with_error(con);
		}

		MYSQL_RES *result = mysql_store_result(con);

		if (result == NULL) {
			finish_with_error(con);
		}

		MYSQL_ROW row;
		float somme;
		somme = 0;

		while( (row = mysql_fetch_row(result)) ){
			if( row[0] != NULL ){
				somme = atof(row[0]);
			}		
		}

		return somme;
		mysql_free_result(result);
	}
}


int mysql_nombre_produits(){

	gchar *query = NULL;
	query = g_strconcat("SELECT COUNT(*) FROM produits", NULL);

	if (con == NULL) {
		g_print("Database error.\n");
		exit(1);
	} else {

		if (mysql_query(con, (char *)query) ) 
		{
			finish_with_error(con);
		}

		MYSQL_RES *result = mysql_store_result(con);

		if (result == NULL) {
			finish_with_error(con);
		}

		MYSQL_ROW row;
		int number;

		while( (row = mysql_fetch_row(result)) ){ 
			number = atoi(row[0]);
		}

		return number;
		mysql_free_result(result);
	}
}


const gchar *mysql_derniere_vente(){

	gchar *query = NULL;
	query = g_strconcat("SELECT DATE_FORMAT(date, \"%d/%m/%Y à %T\") AS formated_date FROM factures ORDER BY date DESC LIMIT 0,1", NULL);

	if (con == NULL) {
		g_print("Database error.\n");
		exit(1);
	} else {

		if (mysql_query(con, (char *)query) ) 
		{
			finish_with_error(con);
		}

		MYSQL_RES *result = mysql_store_result(con);

		if (result == NULL) {
			finish_with_error(con);
		}

		MYSQL_ROW row;
		const gchar *date;

		while( (row = mysql_fetch_row(result)) ){ 
			date = g_strconcat(row[0], NULL);
		}

		return date;
		mysql_free_result(result);
	}

}


