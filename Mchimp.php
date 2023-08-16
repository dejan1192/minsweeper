<?php
require __DIR__ . '/vendor/autoload.php';
/**
 * Controller page for the new personalized mailing list
 *
 *
 */
class Mailchimpv3 extends MY_Controller
{

    public $apikey = '01c84f56fa887f9a7bba511253f55335-us7';
    //List ID
    public $listId = 'fce275dd99';
    //
    //mailchimp config
	public $mc_config; 
	public $member_info;
    public $mailchimp;

    function __construct()
    {
        parent::__construct();
        $this->mailchimp = new Mailchimp\MailchimpLists($this->apikey);
        $this->mc_config = require __DIR__ . '/src/mc_config.php';

		$this->load->model('Ml_users_mc');
        $this->load->helper('url');
    }
    /**
     * Index page for the mailing list with the form
     *
     * @param int $uid User ID, only exists if subscriber comes to this page from the mailing list link
     */
    public function index()
    {
        //basic info for the header
        $layout_data['pageTitle'] = "Kursevi.com | Kursevi na e-mail";
        $layout_data['meta_description'] = "Kursevi.com - Primajte na e-mail najnovije kurseve sa sajta Kursevi.com. Najposećeniji sajt za kurseve, strane jezike i obuku u Srbiji.";
        $layout_data['meta_keywords'] = "kursevi, infostud, kursevi jezika, škole jezika, poslovni seminari, kursevi računara, škole računara, MBA, učenje, obuka, znanje, veština, trening, usavršavanje, karijera, prekvalifikacije";

        $this->carabiner->css(array('jquery.multiSelect.css'));
        $this->carabiner->js(array('jquery.multiSelect.js'));

        $this->load->library('form_validation');
        $this->load->library('email');
        $this->load->helper('string'); // need this only for the password creator

        $euid = isset($_GET['euid']) ? $this->input->get('euid') : $this->input->post('euid');
        $email = isset($_GET['email']) ? $this->input->get('email') : $this->input->post('email');
        $kursevi = isset($_GET['kursevi']) ? $this->input->get('kursevi') : $this->input->post('kursevi');
        $novosti = array();

        if (isset($_POST['prijava']))
            $action = 'prijava';
        elseif (isset($_POST['izmena']))
            $action = 'izmena';
        elseif (isset($_POST['odjava']))
            $action = 'odjava';
        else
            $action = 'none';

        $selected_usergroups =  $this->input->post('subgroups') ? $this->input->post('subgroups') : [];
        $selected_oblasti =  $this->input->post('oblasti') ? $this->input->post('oblasti') : [];
        $selected_regioni = $this->input->post('regioni') ? $this->input->post('regioni') : [];

        if (!empty($selected_usergroups) && in_array(1, $selected_usergroups)) {
            $novosti = array('Novosti');
        }

        if ($action != 'odjava') {
			
            if($action != 'izmena'){
                $this->form_validation->set_rules(
                    'email', 
                    'da unesete vašu imejl adresu',
                     'trim|required|valid_email|callback_validate_is_not_subscribed['. $action .']',
                    [ 'callback_validate_is_not_subscribed' => 'Već ste prijavljeni na mejling listu.']
                );
            }

			$this->form_validation->set_rules('subgroups[]', 'da odaberete mejling listu koju želite da primate', 'required');

            if ((!empty($selected_usergroups)) && (in_array(3, $selected_usergroups))) {
                $this->form_validation->set_rules('oblasti[]', 'da odaberete bar jednu oblast', 'required|numeric');
                $this->form_validation->set_rules('regioni[]', 'da odaberete bar jedan region', 'required|numeric');
            }
        }else{
            $this->form_validation->set_rules('subgroups[]', 'da odaberete mejling listu sa koje želite da se odjavite', 'required');
        }

        $validated = $this->form_validation->run();

        if ($kursevi == 'odjava' || $kursevi == 'izmena') {
            
            $this->load_member_info($email);

			if( ($euid == FALSE || $email == FALSE) || !$this->is_subscribed($email) || !$this->euid_matches_mail($euid)){
				 $layout_data['content_body'] = $this->load->view('others/mailing_lista/verify-failed', '', true);
				 return $this->load->view('layouts/main', $layout_data);
			}else{
				 $selected = $this->get_member_interested_groups($email);
			}
        }

        $body_data = array(
            'euid' => $euid,
            'email' => $email,
            'kursevi' => $kursevi,
            'selected_novosti' => isset($selected['novosti']) ? array(1,2) : array(),
            'selected_oblasti' => isset($selected['oblasti']) ?  array_map('intval', $selected['oblasti']) : array(),
            'selected_regioni' => isset($selected['regioni']) ? array_map('intval', $selected['regioni']) : array(),
            'oblasti' => $this->mappings['oblasti_naziv'],
            'regioni' => $this->mappings['regioni_naziv'],
            'action' => $action
        );


        if ($validated) {

			$mc_params = [];
			$mc_params['interests'] = [];

            $unsubscribe = false;
            $this->load_member_info($email);
			switch ($action) {
                case 'prijava':
					$this->map_interests($mc_params['interests'], $selected_oblasti, 'oblasti_niz');
					$this->map_interests($mc_params['interests'], $selected_regioni, 'regioni_niz');
					$this->map_interests($mc_params['interests'], $novosti, 'novosti_niz');
					
					$mc_params['email_type'] = "html";
					$mc_params['status_if_new'] = $this->mailchimp::MEMBER_STATUS_PENDING;
				
                    setcookie('popup_knm', 'yes', time() + (86400 * 365), "/");
                    $view = "others/mailing_lista/thanks-prijava";
                    break;
                case 'izmena':
                    if ((!empty($selected_usergroups)) && (!in_array(3, $selected_usergroups))) {
                        $selected_oblasti = array();
                        $selected_regioni = array();
                    }
					$this->map_interests($mc_params['interests'], $selected_oblasti, 'oblasti_niz');
					$this->map_interests($mc_params['interests'], $selected_regioni, 'regioni_niz');
					$this->map_interests($mc_params['interests'], $novosti, 'novosti_niz');
			
                    $view = "others/mailing_lista/thanks-izmena";
                    break;
                case 'odjava':
                    $view = "others/mailing_lista/thanks-odjava";
                    
                    if (in_array('1,2', $selected_usergroups) && !empty($selected['oblasti'])) {
                        $this->map_interests($mc_params['interests'], [], 'novosti_niz');
                        $unsubscribe_msg = "mejling liste 'Novo na sajtu'";
                    } elseif (in_array(3, $selected_usergroups) && !empty($selected['novosti'])) {
                        $this->map_interests($mc_params['interests'], [], 'oblasti_niz');
                        $this->map_interests($mc_params['interests'], [], 'regioni_niz');
                        $unsubscribe_msg = "mejling liste 'Kursevi na vaš imejl'";
                    }else{
                        $unsubscribe = true;
                        $unsubscribe_msg = "kompletne mejling liste, i nećete primati ni jedan mejl od nas";
                    }
                    break;

                try {

                    $unsubscribe ? 
                            $this->unsubscribe_user() : 
                            $this->mailchimp->addOrUpdateMember($this->listId, $email, $mc_params);

                }catch(Exception $e){
                    log_message('error', $e->getMessage());
                }

                $body_data['unsubscribe_msg'] = $unsubscribe_msg;
                $layout_data['content_body'] = $this->load->view($view, $body_data, true);

            }
        } else {

            if ($kursevi == 'odjava') {
                $layout_data['content_body'] = $this->load->view('others/mailing_lista/index-odjava', $body_data, true);
            } elseif ($kursevi == 'izmena') {
                $layout_data['content_body'] = $this->load->view('others/mailing_lista/index-izmena', $body_data, true);
            } else {
                $layout_data['content_body'] = $this->load->view('others/mailing_lista/index', $body_data, true);
            }
        }
        $this->load->view('layouts/main', $layout_data);
    }
    private function euid_matches_mail($euid){
        return $euid === $this->member_info->unique_email_id;    
    }

	private function map_interests(&$out, $oblast, $section){
		$mappings = isset($this->mc_config[$section]) ? 
			$this->mc_config[$section] : $this->mappings;
		foreach($mappings as $key => $val){
			if( in_array($val, $oblast) ){
				$out[$key] = true;
 			}else{
				$out[$key] = false;
			}
		}
		return $out;
	}
 
     public function validate_is_not_subscribed($email){
        return $this->is_subscribed($email) === false;
     }
	 
	public function load_member_info($input){
		try {
			$this->member_info = $this->mailchimp->getMemberInfo($this->listId, $input);
        } catch (Exception $e) {
            $this->member_info = null;
        }
	}
    public function has_status_unsubscribed(){
        return $this->member_info->status == $this->mailchimp::MEMBER_STATUS_UNSUBSCRIBED;
    }
    public function has_status_pending(){
        return $this->member_info->status == $this->mailchimp::MEMBER_STATUS_PENDING;    
    }
    public function has_status_subscribed(){
       return isset($this->member_info->status) && $this->member_info->status == $this->mailchimp::MEMBER_STATUS_SUBSCRIBED; 
    }

    public function unsubscribe_user($email){
        $this->Ml_users_mc->delete_by_email($email);
        $this->mailchimp->addOrUpdateMember($this->listId, $email, ['status' => $this->mailchimp::MEMBER_STATUS_UNSUBSCRIBED]);
    }
	 
    public function is_subscribed($input)
    {
        if (!isset($this->member_info)) {
            return FALSE;
        }
        $this->has_status_pending() && $this->unsubscribe_user($input);
    
        if ($this->has_status_subscribed()) {
            return TRUE;
        }
        return FALSE;
    }

    function get_member_interested_groups($input)
    {
		$member_interested_groups = [];
		foreach($this->member_info->interests as $int => $val){

			if(isset($this->mc_config['novosti_niz'][$int])  && $val == TRUE){
				$member_interested_groups['novosti'][$this->mc_config['novosti_niz'][$int]] = "Novosti" ;
				continue;
			}
			if( isset($this->mc_config['oblasti_niz'][$int])  && $val == TRUE){	
				$member_interested_groups['oblasti'][$this->mc_config['oblasti_niz'][$int]] = $this->mappings['oblasti_naziv'][$this->mappings['oblasti_niz'][$int]];
				continue;
			}
			if(isset($this->mc_config['regioni_niz'][$int]) && $val == TRUE){
				$member_interested_groups['regioni'][$this->mc_config['regioni_niz'][$int]] = $this->mappings['regioni_naziv'][$this->mappings['regioni_niz'][$int]];
				continue;
			}
		}

        if ($this->member_info->status == 'subscribed') {
            return $member_interested_groups;
        } else {
            //Nije prijavljen na mailing listu (MC)
            return false;
        }
    }
}
