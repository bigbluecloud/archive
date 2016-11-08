function generate() {
    $.ajax({
        url: "/create",
        dataType: "json",
        success: function(response) {
            appendData(response);
        }
    });
}

function appendData(data) {
    $("#content-panel").append(JSON.stringify(data, null, "\t"));
}
