// Parse the Data
function getData(jsonFile, callBack){d3.json(jsonFile).then(data => callBack(data))}

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
    planeData.push({node:element.Node, minCoreness: element.minCoreness, maxCoreness: element.maxCoreness, numNodi:element.NumeroNodi, numNodiR:element.NumeroNodiR, numNodiL:element.NumeroNodiL, numArchi:element.NumeroArchi})
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
                node.NumeroNodiR += child.NumeroNodiR
                node.NumeroNodiL += child.NumeroNodiL
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

function charts(data, planeData, containerID){
    // Remove old SVG if any
    d3.select(containerID).select("#barchart").remove()
    d3.select(containerID).select("#barchart").remove()
    d3.select(containerID).select("#treemap").remove()
    d3.select(containerID).select("br").remove()
    d3.select(containerID).select("br").remove()
    d3.select(containerID).append("br")
    // SVG and scales definition
    var margin =125;
    var rightMargin = 125;
    var width = planeData.length * 50 + margin+rightMargin;
    var maxNodiL = planeData.reduce(function(prev, current){return (prev.numNodiL > current.numNodiL) ? prev : current}).numNodiL;
    var maxNodiR = planeData.reduce(function(prev, current){return (prev.numNodiR > current.numNodiR) ? prev : current}).numNodiR;
    var heightL = Math.log10(maxNodiL)*100+100//800;
    var svg = d3.select(containerID).append("svg").attr("id", "barchart").attr("width", width+rightMargin).attr("height", heightL);
    width = width-margin;
    heightL = heightL-margin;
    xPadding = 0.4;
    
    
    
    var xScale = d3.scaleBand().domain(planeData.map(function(d) { return d.node; })).range([0, width]).padding(xPadding),
        yScaleL = d3.scaleLog().domain([1, maxNodiL]).range([heightL, 0]).nice();
        format = yScaleL.tickFormat(10, "");
        yScaleL.ticks(10).map(format);
    
    var g = svg.append("g").attr("transform", "translate(" + 100 + "," + 100 + ")");
    
    // X axis
    g.append("g").attr("transform", "translate(0," + heightL + ")").call(d3.axisBottom(xScale)
        .tickFormat(function(dName) {let nodeX = planeData.find(d  => d.node==dName); return  nodeX.minCoreness+"-"+nodeX.maxCoreness}))
    
    // Y axis
    g.append("g").call(d3.axisLeft(yScaleL).tickFormat(function (d) {
            return yScaleL.tickFormat(4,d3.format(",d"))(d)
    }))
             .append("text")
             .attr("y", 6)
             .attr("dy", "0.71em")
             .attr("text-anchor", "end")
             .text("value");
    
    // Y axis Right
    var axisYR = g.append("g").call(d3.axisRight(yScaleL).tickFormat(function (d) {
            return yScaleL.tickFormat(4,d3.format(",d"))(d)
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
             .attr("y", function(d) { return yScaleL(d.numNodiL); })
             .attr("minCorness", function(d) { return d.minCoreness; })
             .attr("maxCorness", function(d) { return d.maxCoreness; })
             .attr("width", xScale.bandwidth())
             .attr("height", function(d) { return heightL - yScaleL(d.numNodiL); })
             .attr("fill", function(d){ return colorScale(d.maxCoreness)});
    // ADD <br> element
    d3.select(containerID).append("br")
    
    // Nested Treemap
    
    compute_height_depth(data, 0)
    padding_top = 5
    padding_bottom = 5
    smallest_rect_height = 20
    treemapHeight = data.height*(padding_top + padding_bottom) + smallest_rect_height
    var svg2 = d3.select(containerID).append("svg").attr("id", "treemap").attr("width", width+margin+rightMargin).attr("height", treemapHeight);
    
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
        if (d.children){
            max = d.children.reduce(function(prev, current){return prev.x1 > current.x1 ? prev : current}).x1
            if(max > d.x1)
            d.x1 = max+4*xScale.padding()
            
        }
    });
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
        .attr("height", d => (d.y1 - d.y0))
        .attr("rx", 15);
   
    node.filter(d => d.children).selectAll("tspan")
        .attr("dx", 3)
        .attr("y", 13);

    node.filter(d => !d.children).selectAll("tspan")
        .attr("x", 3)
        .attr("y", (d, i, nodes) => `${(i === nodes.length - 1) * 0.3 + 1.1 + i * 0.9}em`);



        var heightR = Math.log10(maxNodiR)*100+100//800;
        var svgR = d3.select(containerID).append("svg").attr("id", "barchart").attr("width", width+rightMargin).attr("height", heightR).attr("transform","scale(1,-1)");
        heightR = heightR-margin;
        xPadding = 0.4;
        
        var xScale = d3.scaleBand().domain(planeData.map(function(d) { return d.node; })).range([0, width]).padding(xPadding),
            yScaleR = d3.scaleLog().domain([maxNodiR, 1]).range([0,heightR]).nice();
            format = yScaleR.tickFormat(10, "");
            yScaleR.ticks(10).map(format);
        
        var gR = svgR.append("g").attr("transform", "translate(" + 100 + "," + 100 + ")");
        // X axis
        gR.append("g").attr("transform", "translate(0," + heightR + ")").call(d3.axisTop(xScale)
            .tickFormat(function(dName) {let nodeX = planeData.find(d  => d.node==dName); return  nodeX.minCoreness+"-"+nodeX.maxCoreness}))
        // Y axis
        gR.append("g")
            .attr("transform", "translate(0, 0)") // Non è necessario spostare l'asse Y
            .call(d3.axisLeft(yScaleR).tickFormat(function(d) {
                return yScaleR.tickFormat(4, d3.format(",d"))(d);
            }))
            .append("text")
            .attr("x", 6) // Sposta l'etichetta dell'asse Y a destra
            .attr("dy", "0.32em")
            .attr("text-anchor", "start") // Cambia l'allineamento del testo a sinistra
            .text("value");

        gR.selectAll(".tick text")
            .attr("transform", "scale(1, -1)");
        // Y axis Right
        var axisYR = gR.append("g").call(d3.axisRight(yScaleR).tickFormat(function (d) {
                return yScaleR.tickFormat(4,d3.format(",d"))(d)
        }))
                 .attr("transform", "translate("+(width)+",0)")
                 .append("text")
                 .attr("y", 6)
                 .attr("dy", "0.71em")
                 .attr("text-anchor", "end")
                 .text("value");

        gR.selectAll(".tick text")
            .attr("transform", "scale(1, -1)");
        // to add horizontal lines d3.axisRight(yScale).tickSizeOuter(6).tickSizeInner(-width)
        // to change horizontal lines color (?) axisYR.selectAll('line').style("stroke-dasharray", ("1, 1")) 
        // Colors
        var maxCoreness=planeData.reduce(function(prev, current){return (prev.maxCoreness > current.maxCoreness) ? prev : current}).maxCoreness;
        var myScale = d3.scaleLinear().domain([0,maxCoreness]).range([1, 0]);
        var colorScale = function(d){return d3.interpolateViridis(myScale(d))}    

    
    // Bars
    gR.selectAll(".bar")
    .data(planeData)
    .enter().append("rect")
    .attr("class", "bar")
    .attr("x", function(d) { return xScale(d.node); })
    .attr("y", function(d) { return yScaleR(d.numNodiR); })
    .attr("minCorness", function(d) { return d.minCoreness; })
    .attr("maxCorness", function(d) { return d.maxCoreness; })
    .attr("width", xScale.bandwidth())
    .attr("height", function(d) { return heightR - yScaleR(d.numNodiR); })
    .attr("fill", function(d){ return colorScale(d.maxCoreness)});


    return svg2.node();
}
    
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
        console.log(data)
        console.log(planeData)
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
