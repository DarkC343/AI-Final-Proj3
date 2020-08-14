function chessBoard(size) {
	var mydata = JSON.parse(queens);
    let html = `<table style="width:100%; background-color: powderblue;">
        <tr>
            <th>By: Amir Mohammad Biuki</th>
            <th>Shahed University - Mordad 99</th>
            <th>Solving 8-Queens using genetic algorithm</th>
        </tr>
    </table><div style="display:table; margin:0 auto; margin-top: 20px;" class="chessboard">\n`;
	let i = 0;
    for (let row = 0; row < size; row++) {
        html += `  <div>\n`;
        let color = (row % 2 == 0) ? "black" : "white";
        for (let col = 0; col < size; col++) {
            html += `    <span `;
            if (row == i && mydata[i].pos == col) {
                html += `style="height: 70px; width: 70px; background-color: #DC143C; border-radius: 50%;" `;
                ++i;
            }
            html += ` class="${color}">`;
			html += `</span>\n`;
            color = (color == "white") ? "black" : "white";
        }
        html += `  </div>\n`;
    }
    return html + `</div>`;
}