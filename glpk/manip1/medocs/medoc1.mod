/* Première modélisation avec GNU MathProg 
   Il s'agit d'un modèle totalement explicite */

# Déclaration de variables non-négatives
	
	var x1 >= 0 integer;
	var x2 >= 0 integer;
	var x3 >= 0 integer;
	var x4 >= 0 integer;

# Fonction objectif

	maximize profit : 15*x1 + 20*x2 + 4*x3 + 20*x4;

# Contraintes

	s.t. Toxine1 : 20*x1 + 20*x2 + 10*x3 + 40*x4 <= 21;
	s.t. Toxine2 : 10*x1 + 30*x2 + 20*x3 <= 6;
	s.t. Toxine3 : 20*x1 + 40*x2 + 30*x3 + 10*x4 <= 14;

# Résolution (qui est ajoutée en fin de fichier si on ne le précise pas)
	solve;

# Affichage des résultats
	display : x1, x2, x3, x4;	# affichage "standard" des variables
	printf : "x1 = %f, x2 = %f, x3 = %f, x4 = %f\n",x1,x2,x3,x4; # affichage "formaté à la C" des variables
	display : 15*x1 + 20*x2 + 4*x3 + 20*x4; # affichage de la valeur optimale
	
end;
