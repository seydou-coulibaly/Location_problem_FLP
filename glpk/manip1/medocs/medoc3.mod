/* Première modélisation avec GNU MathProg 
   Il s'agit d'un modèle implicite */

# Déclaration des ensembles utilisés

	param tailleM; # nombre de médicaments
	set M := 1..tailleM; # ensemble des indices des médicaments

	param tailleT; # nombre de toxines produites
	set T := 1..tailleT; # ensemble des indices des toxines
	
	param obj{M}; # coefficients de la fonction objectif
	param coeffcontr{T,M}; # coefficients des membres de gauche des contraintes
	param mdroite{T}; # coefficients des membres de droite

# Déclaration de variables non-négatives sous la forme
# d'un tableau de variables indicées sur les médicaments
	
	var x{M} >= 0;

# Fonction objectif

	maximize profit : sum{j in M} obj[j]*x[j];

# Contraintes

	s.t. Toxine{i in T} : sum{j in M} coeffcontr[i,j]*x[j] <= mdroite[i];

# Résolution (qui est ajoutée en fin de fichier si on ne le précise pas)
	solve;

# Affichage des résultats
	display : x;	# affichage "standard" des variables
	display : sum{j in M} obj[j]*x[j]; # affichage de la valeur optimale
	
# Des données peuvent être séparés de la modélisation, on parle alors de modélisation implicite

data;

	param tailleM := 4;

	param tailleT := 3;

	param obj := 1 15 2 20 3 4 4 20;

	param coeffcontr : 1  2  3  4 :=
	                 1 20 20 10 40
	                 2 10 30 20 0
	                 3 20 40 30 10;

	param mdroite := 1 21
	                 2 6
	                 3 14;

end;
