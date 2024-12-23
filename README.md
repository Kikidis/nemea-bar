ΚΙΚΙΔΗΣ ΝΙΚΟΛΑΟΣ ΑΜ: sdi1900279 email: sdi1900279@di.uoa.gr

ΛΕΙΤΟΥΡΓΙΚΑ ΣΥΣΤΗΜΑΤΑ ΤΜ ΠΕΡΙΤΤΩΝ 2024-25



Τρόπος εκτέλεσης προγράμματος:

Αφού βεβαιωθούμε ότι βρισκόμαστε στο σωστό directory, γράφουμε στο terminal την εντολή: make. Με αυτόν τον τρόπο γίνονται compile όλα τα .cpp αρχεία.
Έπειτα, πηγαίνουμε στον φάκελο creator με την εντολή: cd creator (το μονοπάτι πρέπει να είναι /nemea-bar/creator) και τρέχουμε το πρόγραμμα με: ./creator.
Το πρόγραμμα ξεκινάει και τρέχει για Ν επισκέπτες (τους οποίους ορίζουμε εμείς), επιστρέφοντας ένα ID στο terminal. Έπειτα, επιστρέφουμε στον αρχικό κατάλογο με την εντολή: cd .. και, ξανά, με την εντολή: cd monitor (το μονοπάτι πρέπει να είναι /nemea-bar/monitor), μεταβαίνουμε στον φάκελό του. Τρέχουμε τον monitor με την εντολή: ./monitor -s <ID> (όπου <ID> είναι το ID από τον creator). Όλα τα αποτελέσματα του monitor εμφανίζονται στο terminal. Το πρόγραμμα τερματίζει μετά από Ν επισκέπτες.

Περιγραφή:

Το πρόγραμμα έχει γραφτεί σε γλώσσα C++, γιατί μας βοηθάει καλύτερα στην οργάνωση του κώδικα. Αποτελεί μια προσομοίωση, την οποία προσπαθούμε να υλοποιήσουμε με τον καλύτερο δυνατό τρόπο, λαμβάνοντας υπόψη πώς θα λειτουργούσε στην πραγματικότητα.
Ξεκινώντας, έχουμε δημιουργήσει 4 φακέλους, καθένας από τους οποίους περιέχει ένα module, το .h αρχείο και το Makefile του.

Στον creator δημιουργούμε τη shared memory, η οποία είναι κοινόχρηστη από όλο το πρόγραμμα. Ορίζουμε και αρχικοποιούμε τα στοιχεία που αποτελούν την παραγγελία (order), τα στατιστικά (stats) και τους σημαφόρους (semaphores). Δημιουργούμε συναρτήσεις που εκτελούν attach, detach και free στους σημαφόρους και στη μνήμη. Επίσης, δημιουργούμε τους επισκέπτες (visitors) και τον υπεύθυνο υποδοχής (receptionist), καθώς και συναρτήσεις που κατασκευάζουν το αρχείο καταγραφής (LogFile) και προσθέτουν logs.
Ο τρόπος που λειτουργούμε για να υλοποιηθεί σωστά το πρόβλημα βασίζεται στη χρήση σημαφόρων και έχει ως εξής:
-Ο receptionist μπλοκάρεται στον semaphore του και περιμένει να κάνει post κάποιος visitor που έχει υποβάλει παραγγελία. Εξυπηρετεί την παραγγελία και κάνει post στον visitor για να παραλάβει την παραγγελία του, έπειτα ξαναμπλοκάρεται περιμένοντας τον επόμενο visitor.
-Οι visitors, όταν έρχονται, μπλοκάρονται στην είσοδο (entrance), εκτός από έναν. Στην περιοχή των τραπεζιών μπαίνει μόνο ένας visitor τη φορά. Ο visitor ψάχνει να βρει καρέκλα. Αν υπάρχουν κενές θέσεις, κάνει post στην είσοδο· αλλιώς όχι, και προχωρά στην παραγγελία του. Ένας visitor που φεύγει, αν είναι ο τελευταίος, κοιτάζει αν υπάρχουν ελεύθερες θέσεις σε άλλα τραπέζια. Αν υπάρχουν, φεύγει· αν όχι, κάνει post στην είσοδο. Προσπαθούμε πάντα η είσοδος να είναι 0 ή 1, ώστε όποιος μπαίνει να βρίσκει πάντα μια ελεύθερη θέση.

Στον receptionist ξεκινάμε κάνοντας attach στη μνήμη (shared memory). Περιμένουμε κάποιον visitor να κάνει post. Ο receptionist εξυπηρετεί την παραγγελία σύμφωνα με το service order, που είναι ο τυχαίος χρόνος εξυπηρέτησης, ενημερώνει τα στατιστικά (stats) και ειδοποιεί τον visitor να παραλάβει την παραγγελία. Τέλος, αποσυνδέεται από τη μνήμη.

Στον visitor, πάλι, ξεκινάμε κάνοντας attach στη μνήμη. Ο visitor περιμένει στην ουρά για να βρει καρέκλα και, στη συνέχεια, υποβάλλει την παραγγελία του. Αφού μπει, ενημερώνει το πλήθος των επισκεπτών για τα στατιστικά. Στη συνέχεια, ψάχνει σε ποιο τραπέζι θα καθίσει και σε ποια καρέκλα του τραπεζιού. Αφού καθίσει, ενημερώνει πόσες καρέκλες είναι κατειλημμένες. Ένας νέος visitor εισέρχεται στην περιοχή των τραπεζιών μόνο όταν υπάρχει διαθέσιμη καρέκλα. Αυτό εξασφαλίζεται με τη χρήση των κατάλληλων signals στους σημαφόρους. Έπειτα, ο visitor φεύγει από την περιοχή των τραπεζιών, υποβάλλει την παραγγελία στον receptionist, καταναλώνει αυτό που παρέλαβε από το μπαρ και κάνει sleep() με την κλήση της resttime (ο τυχαίος χρόνος εξυπηρέτησης). Μετά, απελευθερώνει την καρέκλα του και ενημερώνει με signals τους κατάλληλους σημαφόρους για την αποχώρησή του. Τέλος, αποσυνδέεται από τη μνήμη. Αξίζει να σημειωθεί ότι χρησιμοποιούμε τρία ρολόγια (t1, t2, t3) που δείχνουν: 
-τη στιγμή πριν μπει στο μαγαζί,
-τη στιγμή που πέρασε την πόρτα του μαγαζιού, και
-τη στιγμή που αποχώρησε.
Τέλος, έχουμε και τη συνάρτηση addToLogFile, η οποία μας ενημερώνει καθ’ όλη τη διάρκεια με logs.

Στον monitor, η λειτουργία του είναι να μας δίνει ένα στιγμιότυπο για τη συγκεκριμένη στιγμή στην προσομοίωση, να μας ενημερώνει με στατιστικά και τα ID των επισκεπτών. Ο monitor ξεκινάει κάνοντας attach στη shared memory, παρουσιάζει τα στατιστικά για την παραγγελία (order), την κατάσταση στα τραπέζια (κάνοντας post και wait στους αντίστοιχους σημαφόρους), και τέλος αποσυνδέεται από τη μνήμη.