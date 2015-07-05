raise .

pack [canvas .main -width 160 -height 160 -bg #000000]

.main create rectangle 10 35 150 65 -fill #222222 -outline #000000
.main create text 50 50  -text "Magnet 1 " -fill #00FF00 -anchor c
.main create rectangle 10 65 150 95 -fill #222222 -outline #000000
.main create text 50 80  -text "Magnet 2 " -fill #00FF00 -anchor c
.main create rectangle 10 95 150 125 -fill #222222 -outline #000000
.main create text 50 110 -text "Magnet 3 " -fill #00FF00 -anchor c

toplevel .ptnmkr
canvas .ptnmkr.main -width 650 -height 160 -bg #000000
button .ptnmkr.send -text "send pattern" -command sendPattern
pack .ptnmkr.main
pack .ptnmkr.send -side bottom

.ptnmkr.main create rectangle 10 35 100 65 -fill #222222 -outline #000000
.ptnmkr.main create text 50 50  -text "Magnet 1 " -fill #00FF00 -anchor c
.ptnmkr.main create rectangle 10 65 100 95 -fill #222222 -outline #000000
.ptnmkr.main create text 50 80  -text "Magnet 2 " -fill #00FF00 -anchor c
.ptnmkr.main create rectangle 10 95 100 125 -fill #222222 -outline #000000
.ptnmkr.main create text 50 110 -text "Magnet 3 " -fill #00FF00 -anchor c

set ctr [list 0 0 0]
set ptn [list [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] \
	     [list 0 0 0] ]

for { set i 0 } { $i < 20 } { incr i } {
    .ptnmkr.main create rectangle [expr $i*25+105] 35 [expr $i*25+125] 65 -fill #000000 -outline #555555 -width 1 -tag ptn0$i
    .ptnmkr.main create rectangle [expr $i*25+105] 65 [expr $i*25+125] 95 -fill #000000 -outline #555555 -width 1 -tag ptn1$i
    .ptnmkr.main create rectangle [expr $i*25+105] 95 [expr $i*25+125] 125 -fill #000000 -outline #555555 -width 1 -tag ptn2$i
}

bind .ptnmkr <ButtonPress> {
    setptn %x %y
}

proc setptn { x y } {
    global ptn
    set tagstr null
    set j 0
    for { set i 0 } { $i < 20 } { incr i } {
	if { $x > [expr $i*25+105] & $x < [expr $i*25+125] } {
	    if { $y > 35 & $y < 65 } {
		set tagstr "ptn0$i"
		set j 0
	    } elseif { $y > 65 & $y < 95  } {
		set tagstr "ptn1$i"
		set j 1
	    } elseif { $y > 95 & $y < 125 } {
		set tagstr "ptn2$i"
		set j 2
	    }
	    break
	}
    }
    if { $tagstr != "null" } {
	if { [lindex [lindex $ptn $i] $j] == 0 } {
	    .ptnmkr.main itemconfigure $tagstr -fill #FF0000
	    set tmplst [lindex $ptn $i]
	    lset tmplst $j 1
	    lset ptn $i $tmplst
	} else {
	    .ptnmkr.main itemconfigure $tagstr -fill #000000
	    set tmplst [lindex $ptn $i]
	    lset tmplst $j 0
	    lset ptn $i $tmplst
	}
    }
}

bind . <Key-q> {quit}
bind . <KeyPress-1> {set1}
bind . <KeyPress-2> {set2}
bind . <KeyPress-3> {set3}
bind . <KeyPress-space> {unsetall}
bind . <Key-p> {sendPattern}

proc sendPattern { } {
    global ptn
    puts stdout "pattern"
    flush stdout
    for { set i 0 } { $i < 20 } { incr i } {
	puts stdout [lindex $ptn $i]
	flush stdout
	#set buf [gets stdin]
    }
    #set buf [gets stdin]
}

proc displayONOFF { } {
    global ctr
    .main delete magnetstatus
    for { set i 0 } { $i < 3 } { incr i } {
	if { [lindex $ctr $i] == 1 } {
	    .main create text 120 [expr 50+$i*30] -text "ON" -tag magnetstatus -fill #FFFF00
	} else {
	    .main create text 120 [expr 50+$i*30] -text "OFF" -tag magnetstatus -fill #0000DD
	}
    }
    
}


proc quit { } {
    puts stdout "exit"
    flush stdout
    exit
}

proc unsetall { } {
    global ctr
    lset ctr 0 0
    lset ctr 1 0
    lset ctr 2 0
}

proc set1 { } {
    global ctr
    if { [lindex $ctr 0] == 0 } {
	lset ctr 0 1
    } else {
	lset ctr 0 0
    }
}

proc set2 { } {
    global ctr
    if { [lindex $ctr 1] == 0 } {
	lset ctr 1 1
    } else {
	lset ctr 1 0
    }
}

proc set3 { } {
    global ctr
    if { [lindex $ctr 2] == 0 } {
	lset ctr 2 1
    } else {
	lset ctr 2 0
    }
}

proc mainLoop { } {
    global ctr
    puts stdout "set"
    flush stdout
    puts stdout $ctr
    flush stdout
    #set buf [gets stdin]
    displayONOFF
    update
    after 10 mainLoop
}

mainLoop
