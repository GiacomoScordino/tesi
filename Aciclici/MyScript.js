// Parse the Data
function getData(jsonFile, callBack){d3.json(jsonFile).then(data => callBack(data))}

function calcolaMaxLevel(jsonData) {
    let maxLevel = 0;

    function ricercaMaxLevel(jsonNode) {
        if (jsonNode.MaxLevel !== undefined) {
            maxLevel = Math.max(maxLevel, jsonNode.MaxLevel);
        }
        if (jsonNode.children && Array.isArray(jsonNode.children)) {
            for (const child of jsonNode.children) {
                ricercaMaxLevel(child);
            }
        }
    }

    ricercaMaxLevel(jsonData);
    return maxLevel;
}

// Add information about height and depth of nodes in the tree data structure
function compute_height_depth(node, depth){
    node.depth = depth
    var maxHeight = 0
    node.children.forEach(child => {
        tempHeight = compute_height_depth(child, depth+1)
        if(tempHeight > maxHeight)
            maxHeight = tempHeight
    })
    node.height = maxHeight
    return node.height+1
}

function compute_maxSubtreeCoreness(node){
    var maxSubtreeCoreness = node.maxCoreness;
    node.children.forEach(child => {
        let tempSubtreeCoreness = compute_maxSubtreeCoreness(child)
        if(tempSubtreeCoreness > maxSubtreeCoreness)
            maxSubtreeCoreness = tempSubtreeCoreness
    })
    node.maxSubtreeCoreness = maxSubtreeCoreness
    return maxSubtreeCoreness;
}
    
// Data Planarization
function planarize(data){
    compute_height_depth(data, 0)
    compute_maxSubtreeCoreness(data)
    planeData = []
    planeData = planarize_rec(data, planeData)
    if(planeData[0].numNodi == 0)
        planeData[0].numNodi=1
    return planeData
}

function planarize_rec(element, planeData){
    planeData.push({node:element.Node, minCoreness: element.minCoreness, maxCoreness: element.maxCoreness, numNodi:element.NumeroNodi, numArchi:element.NumeroArchi, levels:element.Livelli, minLevel:-1, maxLevel:-1})
    if(element.children){
        //element.children.sort(function(a,b){return a.height - b.height});
        element.children.sort(function(a,b){return a.maxSubtreeCoreness - b.maxSubtreeCoreness});
        element.children.forEach(child => {
            planeData = planarize_rec(child, planeData)       
        });
    }
    return planeData
}

function collapse(node, distance){
    //console.log("collapsing node "+node.Node)
    minCor = node.minCoreness
    if(!node.children)  // return if node is a leaf
        return
    new_children = []
    old_children = []
    // For each child if it is collapsed put its children into new_children and change node.maxCoreness and number of vertices and edges, otherwise put child into old_children.
    // If there are no new children (nothing collapsed) then stop and set node.children as old_children. Otherwise set node.children as new_children and collapse again those on node.
    do {
        node.children.forEach(child => {
            if(child.maxCoreness - minCor < distance){
                //console.log("node "+node.Node+" ("+node.minCoreness+"-"+node.maxCoreness+") collapsing with node "+child.Node+" ("+child.minCoreness+"-"+child.maxCoreness+")")
                new_children = new_children.concat(child.children)
                node.NumeroArchi += child.NumeroArchi
                node.NumeroNodi += child.NumeroNodi
               
                if(child.maxCoreness > node.maxCoreness)
                    node.maxCoreness = child.maxCoreness
            }
            else {
                old_children.push(child)
            }
        })
        node.children = new_children
        new_children = []
        
    } while (node.children.length > 0)
    node.children = old_children // set the new children
    // For each children check if its minCoreness has to be updated, as now it is part of the parent
    var maxCor = node.maxCoreness
    node.children.forEach(child => {
        if(child.minCoreness <= maxCor){
            //console.log("maxCor="+maxCor)
            //console.log("child.minCorness="+child.minCoreness)
            //console.log("changing minCorness of node "+ child.Node+" ("+child.minCoreness+"-"+child.maxCoreness+") to "+ (node.maxCoreness+1))
            child.minCoreness = maxCor+1
        }
        collapse(child, distance)    
    });

}

function computeHeights(child){
    for(var i=0;i<child.levels.length;i++){
        if (child.levels[i]>0){
            if(child.minLevel==-1)
                child.minLevel=i;
            if(i>=child.maxLevel)
                child.maxLevel=i;
        }
    }
}

function charts(data, planeData, containerID){
   
    planeData.forEach(child => computeHeights(child));
    // Remove old SVG if any
    d3.select(containerID).select("#barchart").remove()
    d3.select(containerID).select("#treemap").remove()
    d3.select(containerID).select("br").remove()
    d3.select(containerID).select("br").remove()
    d3.select(containerID).append("br")
    // SVG and scales definition
    var margin =125;
    var rightMargin = 125;
    var width = planeData.length * 50 + margin+rightMargin;
    var maxNodi = planeData.reduce(function(prev, current){return (prev.numNodi > current.numNodi) ? prev : current}).numNodi;
    var height = Math.log10(maxNodi)*100+100//800;
    var svg = d3.select(containerID).append("svg").attr("id", "barchart").attr("width", width+rightMargin).attr("height", height);
    width = width-margin;
    height = height-margin;
    xPadding = 0.4;
    var xScale = d3.scaleBand().domain(planeData.map(function(d) { return d.node; })).range([0, width]).padding(xPadding),
        yScale = d3.scaleLog().domain([1, maxNodi]).range([height, 0]).nice();
        format = yScale.tickFormat(10, "");
        yScale.ticks(10).map(format);
    var g = svg.append("g").attr("transform", "translate(" + 100 + "," + 100 + ")");
    // X axis
    g.append("g").attr("transform", "translate(0," + height + ")").call(d3.axisBottom(xScale)
        .tickFormat(function(dName) {let nodeX = planeData.find(d  => d.node==dName); return  nodeX.minCoreness+"-"+nodeX.maxCoreness}))
    // Y axis
    g.append("g").call(d3.axisLeft(yScale).tickFormat(function (d) {
            return yScale.tickFormat(4,d3.format(",d"))(d)
    }))
             .append("text")
             .attr("y", 6)
             .attr("dy", "0.71em")
             .attr("text-anchor", "end")
             .text("value");
    // Y axis Right
    var axisYR = g.append("g").call(d3.axisRight(yScale).tickFormat(function (d) {
            return yScale.tickFormat(4,d3.format(",d"))(d)
    }))
             .attr("transform", "translate("+(width)+",0)")
             .append("text")
             .attr("y", 6)
             .attr("dy", "0.71em")
             .attr("text-anchor", "end")
             .text("value");
    // to add horizontal lines d3.axisRight(yScale).tickSizeOuter(6).tickSizeInner(-width)
    // to change horizontal lines color (?) axisYR.selectAll('line').style("stroke-dasharray", ("1, 1")) 
    // Colors
    var maxCoreness=planeData.reduce(function(prev, current){return (prev.maxCoreness > current.maxCoreness) ? prev : current}).maxCoreness;
    //var colorScale = d3.scaleSequential().domain([0,maxCoreness]).range(['#ffffe5', '#004529']);
    var myScale = d3.scaleLinear().domain([0,maxCoreness]).range([1, 0]);
    var colorScale = function(d){return d3.interpolateViridis(myScale(d))}
    // Bars
    g.selectAll(".bar")
             .data(planeData)
             .enter().append("rect")
             .attr("class", "bar")
             .attr("x", function(d) { return xScale(d.node); })
             .attr("y", function(d) { return yScale(d.numNodi); })
             .attr("minCorness", function(d) { return d.minCoreness; })
             .attr("maxCorness", function(d) { return d.maxCoreness; })
             .attr("width", xScale.bandwidth())
             .attr("height", function(d) { return height - yScale(d.numNodi); })
             .attr("fill", function(d){ return colorScale((d.minCoreness+d.maxCoreness)/2)});
    // ADD <br> element
    d3.select(containerID).append("br")
    // Nested Treemap
    compute_height_depth(data, 0)
    padding_top = 5
    padding_bottom = 5
    smallest_rect_height = 20
    var numLevel = calcolaMaxLevel(data)+1;
    var maxHeight=data.height
    //console.log("numero dei livelli: ");
    //console.log(numLevel);
    treemapHeight = ((data.height+1)*(padding_top + padding_bottom) + smallest_rect_height)*(numLevel)
    var svg2 = d3.select(containerID).append("svg")
                    .attr("id", "treemap")
                    .attr("width", width+margin+rightMargin)
                    .attr("height", treemapHeight+maxHeight*5)
                    //.style("transform", "scaleY(-1)"); 
    function _treemap(data){
      return d3.treemap()
        .size([width, treemapHeight])
        .paddingTop(padding_top)
        .paddingBottom(padding_bottom)
        .tile(d3.treemapDice)
        .round(true)
        (d3.hierarchy(data)
            .sum(d => d.NumeroNodi)
            .sort((a, b) => b.NumeroNodi - a.NumeroNodi)
        )
    }
    const root = _treemap(data);
    let shadow = svg2.append("filter")
        .attr("id", "shadow")
        .append("feDropShadow")
        .attr("flood-opacity", 0.3)
        .attr("dx", 0)
        .attr("stdDeviation", 3);
    root.eachAfter(function(d){d.x0 = xScale(d.data.Node)+100; d.x1 = d.x0+xScale.bandwidth()});  
    root.eachAfter(function(d){
       
        levels= d.data.MaxLevel-d.data.MinLevel-1;
        d.y0=(smallest_rect_height+10*(maxHeight+1))*(d.data.MinLevel)+5*d.depth;
        d.y1= (smallest_rect_height+10*(maxHeight-d.depth))*2+10*(d.data.MaxLevel-d.data.MinLevel)+(smallest_rect_height+10*maxHeight)*levels+12*d.depth;
        //d.y1= (smallest_rect_height+10*(maxHeight-d.depth))*(d.data.MaxLevel-d.data.MinLevel+1)+10*(d.data.MaxLevel-d.data.MinLevel);
        //d.y1=d.data.MaxLevel*smallest_rect_height+((d.data.MaxLevel-d.data.MinLevel)+2)*10*(maxHeight-d.depth);
        //d.y0=d.data.MinLevel*smallest_rect_height+((d.data.MaxLevel-d.data.MinLevel)+1)*5*(maxHeight-d.depth);
        if (d.children){
            max = d.children.reduce(function(prev, current){return prev.x1 > current.x1 ? prev : current}).x1
            if(max > d.x1)
            d.x1 = max+4*xScale.padding()
        }
    });
    const node = svg2.selectAll("g");
    root.eachAfter(function(d){
        var c=0;
        d.grissini=[];
        console.log(d.data.Node)
        console.log(d)
        
        if(d.parent!= null && d.parent.livellifigli==null)
            d.parent.livellifigli=[];//ogni riga livello e ogni colonna apparte la prima indica dove mettere i grissini
        for(let k=0; k<=numLevel;k++){
            d.grissini[k]=0;
        }
        
        let st=(smallest_rect_height+10*(maxHeight+1))*(d.data.MinLevel+1);
        for(let i=0; i<=numLevel;i++){
            if(d.data.Livelli[i]>0){
                c++;
            }
			else{
                let r = svg2.append("rect")
                .attr("fill", colorScale((d.data.minCoreness+d.data.maxCoreness)/2))
                .attr("width", d.x1-d.x0)
				.attr("x", d.x0)
				.attr("y", d.y0 + ((i-c)*st))
                .attr("height", c * st-(10*d.depth))
				.attr("rx",3)
                //.attr("stroke", "black")    // Imposta il colore del contorno a nero
                //.attr("stroke-width", 0.2)
				.attr("opacity",1);
				r.lower();
				//console.log("Nodo: " + colorScale(d.data.maxCoreness) + " | ALTEZZA" + (c*st) + "| d.y0="+d.y0);
                for(let j=i; j<=numLevel;j++){
                    if(d.data.Livelli[j]>0 && (c>0 || d.grissini[i-1]==1)){
                        d.grissini[i]=1;
                        //.log("grissino inserito al livello "+i)
                    }
                }
                c=0;
                let u=0;
                let bool=true;
                if(d.children!=null){
                    while(d.livellifigli[i][u]!=null){
                        if(d.grissini[i]==1 && d.livellifigli[i][u]!=null){
                            console.log("sono qui mio amico "+d.data.minCoreness +" - "+d.data.maxCoreness  )
                            console.log("---")
                            let stick=svg2.append("rect")
                            .attr("fill", colorScale((d.data.minCoreness+d.data.maxCoreness)/2))
                            .attr("width", (d.livellifigli[i][u][1]-d.livellifigli[i][u][0])/5 + 5*(maxHeight-d.depth))
                            .attr("x", d.livellifigli[i][u][0] - 5*(maxHeight-d.depth)/2 + (d.livellifigli[i][u][1]-d.livellifigli[i][u][0])/2.3)
                            .attr("y", d.y0 + ((i)*st)-(10*d.depth))
                            .attr("height",st+(10*d.depth))
                            .attr("rx",0)
                            //.attr("stroke", "black")    // Imposta il colore del contorno a nero
                            //.attr("stroke-width",0.2)
                            .attr("opacity",1);
                            stick.lower();   
                            bool=false;
                        }
                        u++;
                    }
                    if(d.grissini[i]==1 && bool){
                        console.log("sono qui mio caro")
                        let stick=svg2.append("rect")
                        .attr("fill", colorScale((d.data.minCoreness+d.data.maxCoreness)/2))
                        .attr("width", (d.x1-d.x0)/5+ 5*(maxHeight-d.depth))
                        .attr("x", d.x0- 5*(maxHeight-d.depth)/2 + (d.x1-d.x0)/2.3)
                        .attr("y", d.y0 + ((i)*st)-(10*d.depth))
                        .attr("height",st+(10*d.depth))
                        .attr("rx",0)
                        //.attr("stroke", "black")    // Imposta il colore del contorno a nero
                        //.attr("stroke-width",0.2)
                        .attr("opacity",1);
                        stick.lower();
                    }  
                }else{
                    if(d.grissini[i]==1){
                        let stick=svg2.append("rect")
                        .attr("fill", colorScale((d.data.minCoreness+d.data.maxCoreness)/2))
                        .attr("width", (d.x1-d.x0)/5 + 5*(maxHeight-d.depth))
                        .attr("x", d.x0 -  5*(maxHeight-d.depth)/2 +(d.x1-d.x0)/2.3)
                        .attr("y", d.y0 + ((i)*st)-(10*d.depth))
                        .attr("height",st+(10*d.depth))
                        .attr("rx",0)
                        //.attr("stroke", "black")    // Imposta il colore del contorno a nero
                        //.attr("stroke-width", 0.2)
                        .attr("opacity",1);
                        stick.lower();   
                }  
                }
                //scorrere le righe e poi le colonne
                

            }
        }
        if(d.children==null){
            for (let i =0; i<d.grissini.length;i++){
                if (d.parent!= null && !d.parent.livellifigli[i]) {
                    
                    d.parent.livellifigli[i] = [];
                  }
                if(d.grissini[i]>0){
                    d.parent.livellifigli[i].push(([d.x0,d.x1]))
                }
            }
        }else{
            for(let i=0; i<=numLevel;i++){
                if (d.parent!= null && !d.parent.livellifigli[i]) {
                    d.parent.livellifigli[i] = [];
                }
                if(d.grissini[i]>0 && d.livellifigli[i]!=null){
                    for (let j=0; j<d.livellifigli[i].length;j++){
                        d.parent.livellifigli[i].push(([d.livellifigli[i][j][0],d.livellifigli[i][j][1]]))
                        
                    }
                }else if(d.grissini[i]>0  && d.livellifigli[i]==null){
                    console.log("sono qui");
                    d.parent.livellifigli[i].push(([d.x0,d.x1]))
                }
            }
        }
       
        
       
    });
    root.eachAfter(function(d){
        if (d.children==null){
            
        }
        if (d.parent==null){
           // console.log(d.data.Node+" sono una radice perche non ho genitori.")
        }
    });
    /*
    const node = svg2.selectAll("g")
        .data(d3.group(root, d => d.depth))
        .join("g")
        .attr("filter", "url(#shadow)")
        .selectAll("g")
        .data(d => d[1])
        .join("g")
        .attr("transform", d => `translate(${d.x0},${d.y0})`); 
    node.append("rect")
        .attr("id", d => (d.nodeUid = d3.select("node").id))//DOM.uid("node")).id)
        .attr("fill", d => colorScale(d.data.maxCoreness))
        .attr("width", d => (d.x1 - d.x0))
        .attr("height", d => (d.y1))
        .attr("rx", 15)
        .attr("opacity",0.7);
    */
    // Calcola l'altezza media dei rettangoli a ciascun livello
    const levelHeights = [];
    for (let i = 0; i <= numLevel; i++) {
        const centerY = 0.5 * smallest_rect_height + 5 * (maxHeight + 1) + (smallest_rect_height + 10 * (maxHeight + 1)) * i + 1;
        levelHeights.push(centerY);
    }

    var levelGap=levelHeights[1]-levelHeights[0];
    /*
    // Aggiunge linee orizzontali per ogni livello dei nodi
    for(let i=0; i<levelHeights.length;i++){
        console.log(levelHeights[i]);
        node.append("line")
            .attr("x1", 0) // Inizio della linea all'estremità sinistra del rettangolo
            .attr("x2", d => (d.x1 - d.x0)) // Fine della linea all'estremità destra del rettangolo
            .attr("y1", d=>levelHeights[i]- ((levelGap)*d.data.MinLevel)-5*d.depth )// Specifica la posizione Y desiderata
            .attr("y2",d=>levelHeights[i]- ((levelGap)*d.data.MinLevel)-5*d.depth )// Specifica la posizione Y desiderata
            .attr("stroke", "black") // Colore della linea (puoi cambiare "red" con il colore desiderato)
            .attr("stroke-width", 1)
            .attr("stroke-dasharray", "2.5") // Larghezza della linea (puoi cambiare "2" con la larghezza desiderata)
            .style("display", d => d.data.Livelli[i]<1? "block" : "none");
        }
    // Aggiunge linee orizzontali tratteggiate per ogni livello dei nodi assente
    for(let i=0; i<levelHeights.length;i++){
    node.append("line")
        .attr("x1", 0) // Inizio della linea all'estremità sinistra del rettangolo
        .attr("x2", d => (d.x1 - d.x0)) // Fine della linea all'estremità destra del rettangolo
        .attr("y1", d=>levelHeights[i]- (levelGap*d.data.MinLevel)-5*d.depth )// Specifica la posizione Y desiderata
        .attr("y2",d=>levelHeights[i]- (levelGap*d.data.MinLevel)-5*d.depth )// Specifica la posizione Y desiderata
        .attr("stroke", "black") // Colore della linea (puoi cambiare "red" con il colore desiderato)
        .attr("stroke-width", 2) // Larghezza della linea (puoi cambiare "2" con la larghezza desiderata)
        .style("display", d => d.data.Livelli[i]>0? "block" : "none");
    }
    //aggiunte linee ai bordi dei rettangoli
    for (let i = 0; i <numLevel ; i++) {
        const centerY = levelHeights[i];
        svg2.append("line")
            .attr("x1", root.x0-50) // Inizio della linea all'estremità sinistra del rettangolo radice
            .attr("x2", d => root.x0) // Fine della linea all'estremità destra del rettangolo radice
            .attr("y1", centerY) // Posizione Y centrata
            .attr("y2", centerY) // Posizione Y centrata
            .attr("stroke", "black") // Colore delle linee
            .attr("stroke-width", 2); // Larghezza delle linee
    }
    for (let i = 0; i < numLevel; i++) {
        const centerY = levelHeights[i];
        svg2.append("line")
            .attr("x1", root.x1) // Inizio della linea all'estremità sinistra del rettangolo radice
            .attr("x2", root.x1+50) // Fine della linea all'estremità destra del rettangolo radice
            .attr("y1", centerY) // Posizione Y centrata
            .attr("y2", centerY) // Posizione Y centrata
            .attr("stroke", "black") // Colore delle linee
            .attr("stroke-width", 2); // Larghezza delle linee
    }

*/
// Aggiungi linee orizzontali per ogni livello dei nodi

for (let i = 0; i <= numLevel - 1; i++) {
    const centerY = levelHeights[i];
    svg2.append("line")
        .attr("x1", root.x0) // Inizio della linea all'estremità sinistra del rettangolo radice
        .attr("x2", root.x1) // Fine della linea all'estremità destra del rettangolo radice
        .attr("y1", centerY) // Posizione Y centrata
        .attr("y2", centerY) // Posizione Y centrata
        .attr("stroke", "black") // Colore delle linee
        .attr("stroke-width", 1); // Larghezza delle linee
}

// Aggiungi il testo
for (let i = 0; i < numLevel; i++) {
    const centerY = levelHeights[i];
    svg2.append("text")
    .attr("x", root.x0 - 10) // Posizione X a sinistra della linea
    .attr("y", centerY-3) // Posizione Y centrata
    .attr("text-anchor", "end") // Allinea il testo a destra
    .text(i); // Testo con il valore del livello
}
    node.filter(d => d.children).selectAll("tspan")
        .attr("dx", 3)
        .attr("y", 13);
    node.filter(d => !d.children).selectAll("tspan")
        .attr("x", 3)
        .attr("y", (d, i, nodes) => `${(i === nodes.length - 1) * 0.3 + 1.1 + i * 0.9}em`);
    return svg2.node();
}
    //     1/2 dimensioneRettangolo + 5*altezza + (dimensioneRettangolo+10*(altezza+1))*livello+1
function createButton(containerID){
    let container = d3.select(containerID)
    container.text("Scale 1:")
    container.append("input").attr("type", "number").attr("id", "scale").attr("value", "1").attr("min", "1")
    container.append("input").attr("type", "button").attr("id", "scaleChangeButton").attr("value", "Collapse").attr("onclick", "init(charts)")
    container.append("br")
}   
function init(callBack){
    dist = parseInt(d3.select("#scale").property("value"));
    getData("output.json", function(data){
        collapse(data, dist)
        var planeData = planarize(data);
        callBack(data, planeData, "#container")
    
    })
}
$(document).ready(function(){
    createButton("#container")
    getData("output.json", function(data){
        var originalData = JSON.parse(JSON.stringify(data));
        var length = 0;
        var dist = 1;
        collapse(data, dist); 
        length = planarize(data).length
        if (length < 30){
            init(charts)
        }
        else{
            var min = 1;
            var max = 50;
            while(length>30){
                data = JSON.parse(JSON.stringify(originalData));
                collapse(data, max);
                length = planarize(data).length;
                if(length>30){
                    min = max;
                    max += 50;
                }
            }
            while(min<=max){
                var middle = Math.floor((min + max)/2);
                data = JSON.parse(JSON.stringify(originalData));
                collapse(data, middle);
                length = planarize(data).length;
                if(length<30){
                    max = middle-1;
                }
                else{ if(length>30){
                        min = middle+1;
                    }
                    else{
                        min = middle+1;
                        max = middle;
                    }
                }
            }
            d3.select("#scale").attr("value", max.toString())
            init(charts)
        }
    })
})
/*
$(document).ready(function(){
    createButton("#container")
    init(charts)
})
*/
