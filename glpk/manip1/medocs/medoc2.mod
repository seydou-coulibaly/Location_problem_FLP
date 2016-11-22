/* Première modélisation avec GNU MathProg 
   Il s'agit d'un modèle presque totalement explicite */

# Déclaration des ensembles utilisés

	param tailleM; # nombre de médicaments
	set M := 1..tailleM; # ensemble des indices des médicaments

# Déclaration de variables non-négatives sous la forme
# d'un tableau de variables indicées sur les médicaments
	
	var x{M} >= 0;

# Fonction objectif

	maximize profit : 15*x[1] + 20*x[2] + 4*x[3] + 20*x[4];

# Contraintes

	s.t. Toxine1 : 20*x[1] + 20*x[2] + 10*x[3] + 40*x[4] <= 21;
	s.t. Toxine2 : 10*x[1] + 30*x[2] + 20*x[3] <= 6;
	s.t. Toxine3 : 20*x[1] + 40*x[2] + 30*x[3] + 10*x[4] <= 14;

# Résolution (qui est ajoutée en fin de fichier si on ne le précise pas)
	solve;

# Affichage des résultats
	display : x;	# affichage "standard" des variables
	display : 15*x[1] + 20*x[2] + 4*x[3] + 20*x[4]; # affichage de la valeur optimale
	
# Des données peuvent être séparés de la modélisation, on parle alors de modélisation implicite

data;

	param tailleM := 4;

end;
