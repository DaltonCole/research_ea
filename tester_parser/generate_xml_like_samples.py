from random import choice, randint
from string import ascii_lowercase

samples = 100

for i in range(samples):
	with open("./xml_like_samples/" + str(i) + ".txt", 'w') as file:
		label = ''
		for c in range(randint(1, 26)):
			label += choice(ascii_lowercase)
		internal = ''
		for c in range(randint(1, 26)):
			internal += choice(ascii_lowercase)

		word = '<' + label + '>' + internal + '</' + label + '>'

		file.write(word)